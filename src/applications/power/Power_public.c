/**
 * @file Power_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application public implementation
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include "util/debug/Debug_public.h"
#include "drivers/timer/Timer_public.h"
#include "system/data_pool/DataPool_public.h"
#include "applications/power/Power_public.h"
#include "applications/power/Power_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Power_init(void) {
    
    /* Check that required modules are initialised. Note EPS is not required to
     * be init since it must go through it's state machine to initialise. */
    if (!DP.INITIALISED ||
        !DP.EVENTMANAGER.INITIALISED ||
        !DP.MEMSTOREMANAGER.INITIALISED ||
        !DP.EPS.INITIALISED
    ) {
        DEBUG_ERR("Required module not initialised");
        DP.POWER.ERROR_CODE = POWER_ERROR_DEPENDENCY_NOT_INIT;
        return false;
    }

    /* Set the flags requesting updates to the config, ocp state, and hk
     * reading, so that they are done as soon as the EPS is initialised. Also
     * unset the EPS_OCP_STATE_CORRECT flag so that Mission knows it's not safe
     * to enter a full OPMODE. */
    DP.POWER.UPDATE_EPS_CFG = true;
    DP.POWER.UPDATE_EPS_OCP_STATE = true;
    DP.POWER.UPDATE_EPS_HK = true;
    DP.POWER.EPS_OCP_STATE_CORRECT = false;

    return true;
}

bool Power_step(void) {
    ErrorCode error = ERROR_NONE;
    bool is_event_raised = false;

    /* FIXME: Remove dependence on CFU? */

    #if 0
    /* If there is a mode change underway we raise the update OCP rail flag, so
     * that this mode change is reflected in the EPS OCP rails. Also unset the
     * EPS_OCP_STATE_CORRECT flag, as this is used by Mission to check if the
     * change has been completed. */
    if (!EventManager_is_event_raised(
        EVT_MISSION_OPMODE_CHANGE_STARTED,
        &is_event_raised
    )) {
        DEBUG_ERR("EventManager couldn't check for opmode changed event");
        DP.POWER.ERROR_CODE = POWER_ERROR_EVENTMANAGER_ERROR;
        return false;
    }
    if (is_event_raised) {
        DP.POWER.OPMODE_CHANGE_IN_PROGRESS = true;
        DP.POWER.UPDATE_EPS_OCP_STATE = true;
        DP.POWER.EPS_OCP_STATE_CORRECT = false;
    }
    #endif

    /* If the configuration has been changed in the previous cycle disable the
     * old task timer and clear the event in the data pool. This is required
     * because a config change may result in the task period being changed,
     * therefore we want to restart the timer just in case the period has also
     * changed. Also set the flag to update the EPS's config itself. */
    if (!EventManager_is_event_raised(
        EVT_MEMSTOREMANAGER_CFG_UPDATE_SUCCESS,
        &is_event_raised
    )) {
        DEBUG_ERR("EventManager couldn't check for config change event");
        DP.POWER.ERROR_CODE = POWER_ERROR_EVENTMANAGER_ERROR;
        return false;
    }
    if (is_event_raised) {
        /* If there's already a timer registered disable it and clear the saved
         * event. This will cause a new timer to be started in the next 
         * section. If there isn't a timer registered don't do anything. */
        if (DP.POWER.TASK_TIMER_EVENT != EVT_NONE) {
            error = Timer_disable(DP.POWER.TASK_TIMER_EVENT);
            if (error != ERROR_NONE) {
                DEBUG_ERR("Couldn't disable Power task timer");
                DP.POWER.ERROR_CODE = POWER_ERROR_TASK_TIMER_NOT_DISABLED;
                DP.POWER.TIMER_ERROR_CODE = error;
                return false;
            }
            DP.POWER.TASK_TIMER_EVENT = EVT_NONE;
        }

        /* Raise the flag to update the config, so that the EPS will have any
         * config changes propagated */
        DP.POWER.UPDATE_EPS_CFG = true;
    }

    /* Start the task timer if it hasn't already been started. The EPS health
     * check task runs in all modes except CFU, since updating the CFU will
     * affect the behaviour of the module. Therefore we don't start if we're in
     * CFU */
    if ((DP.OPMODEMANAGER.OPMODE != OPMODEMANAGER_OPMODE_CONFIG_FILE_UPDATE)
        &&
        (DP.POWER.TASK_TIMER_EVENT != EVT_NONE)
    ) {
        error = Timer_start_periodic(
            (double)CFG.POWER_TASK_TIMER_DURATION_S,
            &DP.POWER.TASK_TIMER_EVENT
        );
        if (error != ERROR_NONE) {
            DEBUG_ERR("Couldn't start Power task timer");
            DP.POWER.ERROR_CODE = POWER_ERROR_TASK_TIMER_NOT_STARTED;
            DP.POWER.TIMER_ERROR_CODE = error;
            return false;
        }
    }

    /* Check for firing of the task timer event. As with the above we only do
     * this if we're not in CFU mode. */
    if (DP.OPMODEMANAGER.OPMODE != OPMODEMANAGER_OPMODE_CONFIG_FILE_UPDATE) {
        /* Poll for the event */
        if (!EventManager_poll_event(
            DP.POWER.TASK_TIMER_EVENT,
            &is_event_raised
        )) {
            DEBUG_ERR(
                "EventManager couldn't poll for the task timer event (DP.POWER.TASK_TIMER_EVENT = 0x%04X)", 
                DP.POWER.TASK_TIMER_EVENT
            );
            DP.POWER.ERROR_CODE = POWER_ERROR_EVENTMANAGER_ERROR;
            return false;
        }

        /* If the event has fired */
        if (is_event_raised) {
            /* Raise a new update EPS request. This will mean that if something
             * else has also requested an EPS update we won't get two requests
             * coming through in quick succession. */
            Power_request_eps_hk();
        }
    }

    /* Check for the event that signals the EPS has finished a command. This
     * comes before any raising of new commands so we can clear the EPS command
     * status flag first, allowing us to set new commands in this cycle. */
    if (!EventManager_poll_event(
        EVT_EPS_COMMAND_COMPLETE,
        &is_event_raised
    )) {
        DEBUG_ERR(
            "EventManager couldn't poll for the Eps command complete event"
        );
        DP.POWER.ERROR_CODE = POWER_ERROR_EVENTMANAGER_ERROR;
        return false;
    }
    if (is_event_raised) {
        /* If the command failed retry it, and increment the number of failed
         * counters */
        if (DP.EPS.COMMAND_STATUS == EPS_COMMAND_FAILURE) {
            /* Check if the number of failed commands is above the limit */
            if (DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS 
                > 
                CFG.POWER_MAX_NUM_FAILED_EPS_COMMANDS
            ) {
                /* TODO: What to do here, could reboot the EPS but that would
                 * also reboot the OBC (probably)? Remember to update comment
                 * in config data struct too */
                DEBUG_ERR("Maximum number of failed EPS commands exceeded");
            }

            /* Decide which command to retry.
             * NOTE: pragma used to disable warning about not using all the
             * enum values in the switch. We cant send TM types to the EPS. */
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wswitch-enum"
            switch (DP.POWER.LAST_EPS_COMMAND) {
                case EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA:
                    DP.POWER.UPDATE_EPS_HK = true;
                    break;
                case EPS_UART_DATA_TYPE_TC_SEND_BATT_CMD:
                    /* TODO: how to deal with batt commands? */
                    break;
                case EPS_UART_DATA_TYPE_TC_SET_CONFIG:
                    DP.POWER.UPDATE_EPS_CFG = true;
                    break;
                case EPS_UART_DATA_TYPE_TC_SET_OCP_STATE:
                    DP.POWER.UPDATE_EPS_OCP_STATE = true;
                    break;
                default:
                    DEBUG_ERR(
                        "Unrecognised last EPS command %d",
                        DP.POWER.LAST_EPS_COMMAND
                    );
                    DP.POWER.ERROR_CODE 
                        = POWER_ERROR_UNRECOGNISED_LAST_EPS_COMMAND;
                    return false;
            }
            #pragma GCC diagnostic pop

            /* Increment the number of failed commands */
            DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS++;
        }
        /* Or if the command succeeded clear the failures counter */
        else if (DP.EPS.COMMAND_STATUS == EPS_COMMAND_SUCCESS) {
            DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS = 0;

            /* If the last command was an OCP update set the OCP correct flag */
            if (DP.POWER.LAST_EPS_COMMAND 
                == EPS_UART_DATA_TYPE_TC_SET_OCP_STATE
            ) {
                DP.POWER.EPS_OCP_STATE_CORRECT = true;

                /* If this was part of an opmode change emmit the change
                 * complete event and reset the change flag */
                if (DP.POWER.OPMODE_CHANGE_IN_PROGRESS) {
                    if (!EventManager_raise_event(
                        EVT_POWER_OPMODE_CHANGE_OCP_STATE_CHANGE_COMPLETE
                    )) {
                        DEBUG_ERR(
                            "Couldn't raise OPMODE change complete event"
                        );
                        DP.POWER.ERROR_CODE 
                            = POWER_ERROR_EPS_SET_OCP_STATE_FAILED;
                        return false;
                    }
                    DP.POWER.OPMODE_CHANGE_IN_PROGRESS = false;
                }
            }
        }
        else {
            /* If it's anything else raise a warning, this indicates something
             * is going wrong in the state machine */
            DEBUG_WRN(
                "EPS command complete event was raised, but command status is not success or failure"
            );
        }

        /* Clear the EPS command status flag */
        DP.EPS.COMMAND_STATUS = EPS_COMMAND_NONE;
    }

    /* If there's a request to update the EPS config process it. This takes
     * priority over any other command as it will affect the execution of the
     * others.  */
    if (DP.POWER.UPDATE_EPS_CFG) {
        /* If the EPS is still working on a command (or we haven't read the
         * latest command status stop wait for it to be processed) */
        if (DP.EPS.COMMAND_STATUS == EPS_COMMAND_NONE) {
            /* Send the config to the EPS */
            if (!Eps_send_config()) {
                DEBUG_ERR("Error sending new EPS configuration");
                DP.POWER.ERROR_CODE = POWER_ERROR_EPS_SEND_CONFIG_FAILED;
                return false;
            }

            /* Set the last command to the EPS */
            DP.POWER.LAST_EPS_COMMAND = EPS_UART_DATA_TYPE_TC_SET_CONFIG;

            /* If the request is sent successfully clear the update flag */
            DP.POWER.UPDATE_EPS_CFG = false;
            DEBUG_TRC("Updated EPS config sent");
        }
        else {
            DEBUG_TRC("EPS CFG update requested but EPS is executing a command");
        }
    }

    /* If there's a request to update the EPS OCP state process it */
    if (DP.POWER.UPDATE_EPS_OCP_STATE) {
        /* If the EPS is still working on a command (or we haven't read the
         * latest command status stop wait for it to be processed) */
        if (DP.EPS.COMMAND_STATUS == EPS_COMMAND_NONE) {
            /* Get the OCP state based on the mode at the end of the current
             * state change. */
            DP.POWER.REQUESTED_OCP_STATE = Power_get_ocp_state_for_op_mode(
                CFG.POWER_OP_MODE_OCP_STATE_CONFIG,
                DP.OPMODEMANAGER.NEXT_OPMODE
            );

            /* Send the new state to the Eps */
            if (!Eps_set_ocp_state(DP.POWER.REQUESTED_OCP_STATE)) {
                DEBUG_ERR("Error sending updated OCP state to EPS");
                DP.POWER.ERROR_CODE = POWER_ERROR_EPS_SET_OCP_STATE_FAILED;
                return false;
            }

            /* Set the last command to the EPS */
            DP.POWER.LAST_EPS_COMMAND = EPS_UART_DATA_TYPE_TC_SET_OCP_STATE;

            /* If the request is sent successfully clear the update flag */
            DP.POWER.UPDATE_EPS_OCP_STATE = false;
            DEBUG_TRC("Updated EPS OCP state sent");
            DEBUG_DBG(
                "DP.POWER.REQUESTED_OCP_STATE: %d%d%d%d%d%d",
                DP.POWER.REQUESTED_OCP_STATE.radio_tx,
                DP.POWER.REQUESTED_OCP_STATE.radio_rx_camera,
                DP.POWER.REQUESTED_OCP_STATE.eps_mcu,
                DP.POWER.REQUESTED_OCP_STATE.obc,
                DP.POWER.REQUESTED_OCP_STATE.gnss_rx,
                DP.POWER.REQUESTED_OCP_STATE.gnss_lna
            );
        }
        else {
            DEBUG_TRC("EPS OCP update requested but EPS is executing a command");
        }
    }

    /* If there's a request to update the EPS HK data process it */
    if (DP.POWER.UPDATE_EPS_HK) {
        /* If the EPS is still working on a command (or we haven't read the
         * latest command status stop wait for it to be processed) */
        if (DP.EPS.COMMAND_STATUS == EPS_COMMAND_NONE) {
            /* Send the request to the Eps */
            if (!Eps_collect_hk_data()) {
                DEBUG_ERR("Error requesting new EPS HK packet");
                DP.POWER.ERROR_CODE = POWER_ERROR_EPS_COLLECT_HK_FAILED;
                return false;
            }

            /* Set the last command to the EPS */
            DP.POWER.LAST_EPS_COMMAND = EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA;

            /* If the request is sent successfully clear the update flag */
            DP.POWER.UPDATE_EPS_HK = false;
            DEBUG_TRC("Update EPS HK request sent");
        }
        else {
            DEBUG_TRC("EPS HK update requested but EPS is executing a command");
        }
    }

    /* If there's an updated EPS HK packet perform the generic low voltage
     * check, provided we are not in CFU. Switching to LP in CFU is dangerous
     * and could leave the spacecraft in a misconfigured state. */
    if (DP.OPMODEMANAGER.OPMODE != OPMODEMANAGER_OPMODE_CONFIG_FILE_UPDATE) {
        /* Note we use is_event_raised here not poll as some others may be
         * looking for this event too */
        if (!EventManager_is_event_raised(
            EVT_EPS_NEW_HK_DATA,
            &is_event_raised
        )) {
            DEBUG_ERR(
                "EventManager couldn't check for the Eps new HK data event"
            );
            DP.POWER.ERROR_CODE = POWER_ERROR_EVENTMANAGER_ERROR;
            return false;
        }
        if (is_event_raised) {
            /* TODO: battery voltage check */
        }
    }

    return true;
}

void Power_request_eps_hk(void) {
    /* If there's already a request waiting we will issue a warning, but won't
     * do anything */
    if (DP.POWER.UPDATE_EPS_HK) {
        DEBUG_WRN("EPS HK request made when a pending request sill exists");
    }

    DP.POWER.UPDATE_EPS_HK = true;
}

void Power_request_ocp_state_for_next_opmode(void) {
    DP.POWER.REQUESTED_OCP_STATE = Power_get_ocp_state_for_op_mode(
        CFG.POWER_OP_MODE_OCP_STATE_CONFIG,
        DP.OPMODEMANAGER.NEXT_OPMODE
    );
    DP.POWER.UPDATE_EPS_OCP_STATE = true;
    DP.POWER.OPMODE_CHANGE_IN_PROGRESS = true;
}