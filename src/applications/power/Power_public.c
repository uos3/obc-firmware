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
    
    /* Check that required modules are initialised */
    if (!DP.INITIALISED ||
        !DP.EVENTMANAGER.INITIALISED ||
        !DP.MEMSTOREMANAGER.INITIALISED ||
        !DP.EPS.INITIALISED
    ) {
        DEBUG_ERR("Required module not initialised");
        DP.POWER.ERROR.code = POWER_ERROR_DEPENDENCY_NOT_INIT;
        DP.POWER.ERROR.p_cause = NULL;
        return false;
    }

    /* Set the flags requesting updates to the config, ocp state, and hk
     * reading, so that they are done as soon as the EPS is initialised. Also
     * unset the EPS_OCP_STATE_CORRECT flag so that OpModeManager knows it's 
     * not safe to enter a full OPMODE. */
    DP.POWER.UPDATE_EPS_CFG = true;
    DP.POWER.UPDATE_EPS_OCP_STATE = true;
    DP.POWER.UPDATE_EPS_HK = true;
    DP.POWER.LOW_POWER_STATUS = POWER_LOW_POWER_STATUS_READ_IN_PROGRESS;
    DP.POWER.EPS_OCP_STATE_CORRECT = false;

    return true;
}

bool Power_step(void) {
    ErrorCode error = ERROR_NONE;
    Eps_CommandStatus eps_command_status;

    /* NOTE: previously there was a test on whether or not there was a config
     * file change, and if so we would restart the task timer. However it's
     * intended that a CFU will trigger a reset of the OBC, so there's no need
     * to do this any more */

    /* Start the task timer if it hasn't already been started. */
    if (DP.POWER.TASK_TIMER_EVENT != EVT_NONE) {
        error = Timer_start_periodic(
            (double)CFG.POWER_TASK_TIMER_DURATION_S,
            &DP.POWER.TASK_TIMER_EVENT
        );
        if (error != ERROR_NONE) {
            DEBUG_ERR("Couldn't start Power task timer");
            DP.POWER.ERROR.code = POWER_ERROR_TASK_TIMER_NOT_STARTED;
            DP.POWER.ERROR.p_cause = &DP.POWER.TIMER_ERROR;
            DP.POWER.TIMER_ERROR.code = error;
            DP.POWER.TIMER_ERROR.p_cause = NULL;
            return false;
        }
    }

    /* Check for firing of the task timer event. */
    if (EventManager_poll_event(DP.POWER.TASK_TIMER_EVENT)) {
        /* Raise a new update EPS request. This will mean that if something
         * else has also requested an EPS update we won't get two requests
         * coming through in quick succession. */
        Power_request_eps_hk();
    }

    /* Check for the event that signals the EPS has finished a command. This
     * comes before any raising of new commands so we can clear the EPS command
     * status flag first, allowing us to set new commands in this cycle. */
    if (EventManager_poll_event(EVT_EPS_COMMAND_COMPLETE)) {
        /* Get a copy of the status flag before unsetting it. We do this before
         * processing the result so that a failure at this stage will not leave
         * the flag raised, which would result in multiple reattempts with the
         * same data. */
        eps_command_status = DP.EPS.COMMAND_STATUS;
        DP.EPS.COMMAND_STATUS = EPS_COMMAND_NONE;

        /* If the command failed retry it, and increment the number of failed
         * counters */
        if (eps_command_status == EPS_COMMAND_FAILURE) {
            /* Check if the number of failed commands is above the limit */
            if (DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS 
                >= 
                POWER_MAX_NUM_FAILED_EPS_COMMANDS
            ) {
                /* FIXME: What to do here, could reboot the EPS but that would
                 * also reboot the OBC (probably)? Remember to update comment
                 * in config data struct with how this is resolved */
                /* This is probably a case where we want to switch to safe
                 * mode, because we have vastly degratded capability if the EPS
                 * isn't responding. */
                DEBUG_ERR("Maximum number of failed EPS commands exceeded");
                DP.POWER.ERROR.code = POWER_MAX_NUM_EPS_FAILED_CMDS_EXCEEDED;
                DP.POWER.ERROR.p_cause = &DP.EPS.ERROR;
                return false;
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
                    DP.POWER.SEND_BATT_TC = true;
                    break;
                case EPS_UART_DATA_TYPE_TC_SET_CONFIG:
                    DP.POWER.UPDATE_EPS_CFG = true;
                    break;
                case EPS_UART_DATA_TYPE_TC_SET_OCP_STATE:
                    DP.POWER.UPDATE_EPS_OCP_STATE = true;
                    break;
                case EPS_UART_DATA_TYPE_TC_RESET_OCP:
                    DP.POWER.SEND_RESET_OCP_TC = true;
                    break;
                default:
                    DEBUG_ERR(
                        "Unrecognised last EPS command %d",
                        DP.POWER.LAST_EPS_COMMAND
                    );
                    DP.POWER.ERROR.code
                        = POWER_ERROR_UNRECOGNISED_LAST_EPS_COMMAND;
                    DP.POWER.ERROR.p_cause = NULL;
                    return false;
            }
            #pragma GCC diagnostic pop

            /* Increment the number of failed commands */
            DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS++;
        }
        /* Or if the command succeeded clear the failures counter */
        else if (eps_command_status == EPS_COMMAND_SUCCESS) {
            /* Reset the number of consecutive failed commands */
            DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS = 0;

            /* Handle the successful last command.
             * NOTE: pragma used to disable warning about not using all the
             * enum values in the switch. We cant send TM types to the EPS. */
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wswitch-enum"
            switch (DP.POWER.LAST_EPS_COMMAND) {
                /* Successfully collected new HK data */
                case EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA:
                    /* TODO: store as TM */

                    /* Do battery voltage check */
                    if (!Power_low_power_status_check()) {
                        DEBUG_ERR("Couldn't perform low power check");

                        /* If the power check fails we raise the acquire eps hk
                         * data again flag, so that we can try again with new
                         * data */
                        Power_request_eps_hk();

                        return false;
                    }
                    break;

                /* Successfully set a new OCP state */
                case EPS_UART_DATA_TYPE_TC_SET_OCP_STATE:

                    /* Confirm that the requested and reported states match */
                    if (!Eps_do_ocp_states_match(
                        &DP.POWER.REQUESTED_OCP_STATE,
                        &DP.EPS.REPORTED_OCP_STATE
                    )) {
                        DEBUG_ERR("Reported OCP state is incorrect");
                        DP.POWER.ERROR.code 
                            = POWER_ERROR_EPS_SET_OCP_STATE_FAILED;
                        DP.POWER.ERROR.p_cause = NULL;

                        /* Retry the set command */
                        DP.POWER.UPDATE_EPS_OCP_STATE = true;
                        
                        /* Increment the number of failed commands */
                        DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS++;

                        return false;
                    }

                    DP.POWER.EPS_OCP_STATE_CORRECT = true;

                    /* If this was part of an opmode change emmit the change
                     * complete event and reset the change flag */
                    if (DP.POWER.OPMODE_CHANGE_IN_PROGRESS) {
                        DP.POWER.OPMODE_CHANGE_IN_PROGRESS = false;
                        if (!EventManager_raise_event(
                            EVT_POWER_OPMODE_CHANGE_OCP_STATE_CHANGE_COMPLETE
                        )) {
                            DEBUG_ERR(
                                "Couldn't raise OPMODE change complete event"
                            );
                            DP.POWER.ERROR.code 
                                = POWER_ERROR_EVENTMANAGER_ERROR;
                            DP.POWER.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
                            return false;
                        }
                    }

                    break;

                /* Successfully reset OCP rail(s) */
                case EPS_UART_DATA_TYPE_TC_RESET_OCP:

                    /* Confirm that the requested and reported states match */
                    if (!Eps_do_ocp_states_match(
                        &DP.POWER.REQUESTED_OCP_STATE,
                        &DP.EPS.REPORTED_OCP_STATE
                    )) {
                        DEBUG_ERR("Reported OCP state is incorrect");
                        DP.POWER.ERROR.code 
                            = POWER_ERROR_EPS_SET_OCP_STATE_FAILED;
                        DP.POWER.ERROR.p_cause = NULL;

                        /* Retry the set command */
                        DP.POWER.UPDATE_EPS_OCP_STATE = true;
                        
                        /* Increment the number of failed commands */
                        DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS++;

                        return false;
                    }

                    DP.POWER.EPS_OCP_STATE_CORRECT = true;

                    /* Clear the rails to reset value */
                    memset(
                        &DP.POWER.OCP_RAILS_TO_RESET,
                        0,
                        sizeof(Eps_OcpState)
                    );

                    /* Emmit the event signalling this */
                    if (!EventManager_raise_event(EVT_POWER_OCP_RESET_SUCCESS)) {
                        DEBUG_ERR("Couldn't raise OCP reset success event");
                        DP.POWER.ERROR.code = POWER_ERROR_EVENTMANAGER_ERROR;
                        DP.POWER.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
                        return false;
                    }

                    break;

                default:
                    /* Not all TCs require action on success, we will just put
                     * a trace here that we didn't do anything with that TC */
                    DEBUG_TRC(
                        "No action on successful EPS TC %d",
                        DP.POWER.LAST_EPS_COMMAND
                    );
                    break;
            }
            #pragma GCC diagnostic pop
        }
        else {
            /* If it's anything else raise a warning, this indicates something
             * is going wrong in the state machine */
            DEBUG_WRN(
                "EPS command complete event was raised, but command status is not success or failure"
            );
        }
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
                DP.POWER.ERROR.code = POWER_ERROR_EPS_SEND_CONFIG_FAILED;
                DP.POWER.ERROR.p_cause = &DP.EPS.ERROR;
                return false;
            }

            /* Set the last command to the EPS */
            DP.POWER.LAST_EPS_COMMAND = EPS_UART_DATA_TYPE_TC_SET_CONFIG;

            /* If the request is sent successfully clear the update flag */
            DP.POWER.UPDATE_EPS_CFG = false;
            DEBUG_TRC("Updated EPS config sent");
        }
        else {
            /*DEBUG_TRC("EPS CFG update requested but EPS is executing a command");*/
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
                CFG.POWER_OPMODE_OCP_STATE_CONFIG,
                DP.OPMODEMANAGER.NEXT_OPMODE
            );

            /* Send the new state to the Eps */
            if (!Eps_set_ocp_state(DP.POWER.REQUESTED_OCP_STATE)) {
                DEBUG_ERR("Error sending updated OCP state to EPS");
                DP.POWER.ERROR.code = POWER_ERROR_EPS_SET_OCP_STATE_FAILED;
                DP.POWER.ERROR.p_cause = &DP.EPS.ERROR;
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
            /*DEBUG_TRC("EPS OCP update requested but EPS is executing a command");*/
        }
    }

    /* If there's a request to reset OCP rail(s) */
    if (DP.POWER.SEND_RESET_OCP_TC) {
        /* If the EPS is still working on a command (or we haven't read the
         * latest command status stop wait for it to be processed) */
        if (DP.EPS.COMMAND_STATUS == EPS_COMMAND_NONE) {
            /* Send the reset command to the EPS */
            if (!Eps_reset_ocp(DP.POWER.OCP_RAILS_TO_RESET)) {
                DEBUG_ERR("Error sending OCP reset command to EPS");
                DP.POWER.ERROR.code = POWER_ERROR_EPS_RESET_OCP_FAILED;
                DP.POWER.ERROR.p_cause = &DP.EPS.ERROR;
            }

            /* Set the last command to the EPS */
            DP.POWER.LAST_EPS_COMMAND = EPS_UART_DATA_TYPE_TC_RESET_OCP;

            /* If the request is sent successfully clear the send flag */
            DP.POWER.SEND_RESET_OCP_TC = false;
            DEBUG_TRC("Reset EPS OCP sent");
            DEBUG_DBG(
                "DP.POWER.OCP_RAILS_TO_RESET: %d%d%d%d%d%d",
                DP.POWER.OCP_RAILS_TO_RESET.radio_tx,
                DP.POWER.OCP_RAILS_TO_RESET.radio_rx_camera,
                DP.POWER.OCP_RAILS_TO_RESET.eps_mcu,
                DP.POWER.OCP_RAILS_TO_RESET.obc,
                DP.POWER.OCP_RAILS_TO_RESET.gnss_rx,
                DP.POWER.OCP_RAILS_TO_RESET.gnss_lna
            );
        }
        else {
            /*DEBUG_TRC("EPS OCP reset requested but EPS is executing a command");*/
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
                DP.POWER.ERROR.code = POWER_ERROR_EPS_COLLECT_HK_FAILED;
                DP.POWER.ERROR.p_cause = &DP.EPS.ERROR;
                return false;
            }

            /* Set the last command to the EPS */
            DP.POWER.LAST_EPS_COMMAND = EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA;

            /* If the request is sent successfully clear the update flag */
            DP.POWER.UPDATE_EPS_HK = false;
            DEBUG_TRC("Update EPS HK request sent");
        }
        else {
            /*DEBUG_TRC("EPS HK update requested but EPS is executing a command");*/
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

    /* Set the low power status update in progress flag, as we are in the 
     * middle of getting the updated EPS HK */
    DP.POWER.LOW_POWER_STATUS = POWER_LOW_POWER_STATUS_READ_IN_PROGRESS;

    DP.POWER.UPDATE_EPS_HK = true;
}

bool Power_reset_ocp(Eps_OcpState rails_to_reset_in) {
    /* First we need to check if there's already a reset pending. Since
     * DP.POWER.OCP_RAILS_TO_RESET will be all false when a reset isn't pending
     * we check for this */
    if (DP.POWER.OCP_RAILS_TO_RESET.radio_tx
        ||
        DP.POWER.OCP_RAILS_TO_RESET.radio_rx_camera
        ||
        DP.POWER.OCP_RAILS_TO_RESET.eps_mcu
        ||
        DP.POWER.OCP_RAILS_TO_RESET.obc
        ||
        DP.POWER.OCP_RAILS_TO_RESET.gnss_rx
        ||
        DP.POWER.OCP_RAILS_TO_RESET.gnss_lna
    ) {
        DEBUG_ERR(
            "Can't request rail reset as the last reset operation hasn't finished yet."
        );
        DP.POWER.ERROR.code = POWER_ERROR_EPS_RESET_OCP_IN_PROGRESS;
        DP.POWER.ERROR.p_cause = NULL;
        return false;
    }

    /* If there isn't one in progress set the input in the datapool and set the
     * send flag */
    DP.POWER.OCP_RAILS_TO_RESET = rails_to_reset_in;
    DP.POWER.SEND_RESET_OCP_TC = true;

    return true;
}

void Power_request_ocp_state_for_next_opmode(void) {
    DP.POWER.REQUESTED_OCP_STATE = Power_get_ocp_state_for_op_mode(
        CFG.POWER_OPMODE_OCP_STATE_CONFIG,
        DP.OPMODEMANAGER.NEXT_OPMODE
    );
    DP.POWER.UPDATE_EPS_OCP_STATE = true;
    DP.POWER.OPMODE_CHANGE_IN_PROGRESS = true;
}