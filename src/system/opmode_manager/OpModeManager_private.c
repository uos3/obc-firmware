/**
 * @file OpModeManager_private.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief OpModeManager private implementations
 * 
 * Task ref: [UT_2.9.10]
 * 
 * @version 0.1
 * @date 2021-03-11
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdbool.h>
#include <stdint.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/timer/Timer_public.h"
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "system/opmode_manager/OpModeManager_public.h"
#include "system/opmode_manager/OpModeManager_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void OpModeManager_activate_apps(void) {
    int i;
    Kernel_AppId app_id;
    
    /* Loop through all app Ids that should be in the current opmode, and
     * activate each one */
    for (i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
        /* Get the app id at this index */
        app_id = CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.OPMODE][i];

        /* If the app ID is zero it is empty, so we skip to the next */
        if (app_id == 0) {
            continue;
        }

        /* Otherwise switch over the possible user apps, and call their
         * activate functions */
        switch (app_id) {
            case APP_ID_BEACON:
                DEBUG_TRC("Activating Beacon app");
                /* TODO: call activate func when defined */
                break;
                
            case APP_ID_DEPLOYMENT:
                DEBUG_TRC("Activating Deployment app");
                /* TODO: call activate func when defined */
                break;

            case APP_ID_PAYLOAD:
                DEBUG_TRC("Activating Payload app");
                /* TODO: call activate func when defined */
                break;

            case APP_ID_PICTURE:
                DEBUG_TRC("Activating Picture app");
                /* TODO: call activate func when defined */
                break;

            default:
                DEBUG_WRN(
                    "OPMODE %d contains an AppId 0x%02X unkown to OpModeManager",
                    DP.OPMODEMANAGER.OPMODE,
                    app_id
                );
                /* TODO: raise error event? */
                break;
        }
    }
}

bool OpModeManager_check_app_trans_reqs(void) {
    
    /* 
     * System app transition requests
     * 
     * Generally speaking system apps don't request graceful transitions, they
     * use the FDIR system to request emergency ones - i.e. Power will request
     * LP mode through the FDIR events.
     * 
     * However this doesn't preclude some system app requesting a transition
     * at some point in the future - if we decide to keep DL mode then comms
     * will be responsible for requesting it. 
     */

    /* 
     * User app transition requests
     * 
     * As any emergency transition requests will be made through the FDIR
     * system we are only checking for graceful transitions. The way each app
     * requests a graceful transition should be through the event system. In
     * addition a check is made that the app is supposed to be running in that
     * mode, this is a safeguard against poorly written apps attempting to take
     * control of the OpModeManager.
     */

    /* TODO: For all user apps:
     *  - Check for transition request event (evt is app specfic)
     *  - Check the app is supposed to be running in the current mode
     *  - Check that the transition is allowed?
     *  - Set the state into graceful transition and set the next opmode
     */

    return true;
}

bool OpModeManager_call_active_app_steps(void) {
    int i;
    Kernel_AppId app_id;

    /* For all apps active in the current mode call their step functions */
    for (i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
        /* Get the app id at this index */
        app_id = CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.OPMODE][i];

        /* If the app ID is zero it is empty, so we skip to the next */
        if (app_id == 0) {
            continue;
        }

        switch (app_id) {
            case APP_ID_BEACON:
                DEBUG_TRC("Stepping Beacon app");
                /* TODO: call step func when defined */
                break;
                
            case APP_ID_DEPLOYMENT:
                DEBUG_TRC("Stepping Deployment app");
                /* TODO: call step func when defined */
                break;

            case APP_ID_PAYLOAD:
                DEBUG_TRC("Stepping Payload app");
                /* TODO: call step func when defined */
                break;

            case APP_ID_PICTURE:
                DEBUG_TRC("Stepping Picture app");
                /* TODO: call step func when defined */
                break;

            default:
                DEBUG_WRN(
                    "OPMODE %d contains an AppId 0x%02X unkown to OpModeManager",
                    DP.OPMODEMANAGER.OPMODE,
                    app_id
                );
                /* TODO: raise error event? */
                break;
        }
    }

    return true;
}

bool OpModeManager_step_graceful_transition(void) {
    int i;
    Kernel_AppId app_id;
    bool app_in_next_mode;
    ErrorCode error;
    bool wait_off_timeout_fired = false;
    bool is_ocp_change_complete = false;

    /* Switch on the current graceful transition */
    switch (DP.OPMODEMANAGER.GRACE_TRANS_STATE) {
        case OPMODEMANAGER_GRACETRANSSTATE_INIT:

            /* Start the transition timeout timer */
            error = Timer_start_one_shot(
                OPMODEMANAGER_GRACE_TRANS_TIMEOUT_S,
                &DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT
            );
            if (error != ERROR_NONE) {
                /* If we couldn't start a timer we will record the RTC value at
                 * this point and compare it in the future. We set the event as
                 * EVT_NONE as this will switch timeout behaviour from checking
                 * for timer event to checking for RTC value. */
                DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT = EVT_NONE;
                /* TODO: record RTC value */
                DEBUG_WRN(
                    "Couldn't register timer for graceful transition timeout, falling back on RTC"
                );
            }


            /* Request stop all user apps which are not active in the next mode */
            for (i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
                /* Reset the app in next mode flag */
                app_in_next_mode = false;

                /* Get the app id */
                app_id = CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.OPMODE][i];

                /* If the app id is 0 skip to the next one */
                if (app_id = 0) {
                    continue;
                }
                else {
                    /* If the app id is 0 or the app is in the next mode skip to 
                     * the next one */
                    if ((app_id == 0)
                        ||
                        (DP.OPMODEMANAGER.APP_IN_NEXT_MODE[i])
                    ) {
                        continue;
                    }
                    else {
                        /* TODO: request off for app */
                    }
                }
            }

            /* set next state */
            DP.OPMODEMANAGER.GRACE_TRANS_STATE 
                = OPMODEMANAGER_GRACETRANSSTATE_WAIT_ACTIVE_OFF;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case OPMODEMANAGER_GRACETRANSSTATE_WAIT_ACTIVE_OFF:

            /* Check for timeout, either using the timer event or if that is
             * None use the RTC time */
            if (DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT != EVT_NONE) {
                if (!EventManager_poll_event(
                    DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT,
                    &wait_off_timeout_fired
                )) {
                    DEBUG_ERR(
                        "EventManager error while polling for graceful transition event timeout"
                    );
                    DP.OPMODEMANAGER.ERROR_CODE = OPMODEMANAGER_ERROR_EVENTMANAGER_ERROR;
                    /* We want to try and guarentee that this will continue, so
                     * we will ignore the error and *assume* that the timeout 
                     * has fired. While this may result in error loss it is
                     * more important to make sure the transition actually
                     * completes. */
                    wait_off_timeout_fired = true;
                }
            }
            else {
                /* TODO: Check RTC */
            }

            /* Check all active apps to make sure they are off, if the timeout
             * hasn't fired */
            if (!wait_off_timeout_fired) {
                for (i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
                    /* Reset the app in next mode flag */
                    app_in_next_mode = false;

                    /* Get the app id */
                    app_id = CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.OPMODE][i];

                    /* If the app id is 0 skip to the next one */
                    if (app_id = 0) {
                        continue;
                    }
                    else {
                        /* If the app id is 0 or the app is in the next mode skip to 
                         * the next one */
                        if ((app_id == 0)
                            ||
                            (DP.OPMODEMANAGER.APP_IN_NEXT_MODE[i])
                        ) {
                            continue;
                        }
                        else {
                            /* TODO: Check that the app's state is OFF. If not 
                             * return now, giving time for the transition to
                             * complete. */
                        }
                    }
                }

                /* If all apps are off disable the timer */
                if (DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT != EVT_NONE) {
                    error = Timer_disable(
                        DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT
                    );
                    if (error != ERROR_NONE) {
                        DEBUG_ERR("Couldn't disable timeout: 0x%04X", error);
                        /* we will just continue, and try to finish the
                         * transition */
                    }
                }
            }
            else {
                /* If the timeout was fired we need to force all non-continuing
                 * and not already off apps to be off */
                for (i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
                    /* Reset the app in next mode flag */
                    app_in_next_mode = false;

                    /* Get the app id */
                    app_id = CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.OPMODE][i];

                    /* If the app id is 0 skip to the next one */
                    if (app_id = 0) {
                        continue;
                    }
                    else {
                        /* If the app id is 0 or the app is in the next mode skip to 
                         * the next one */
                        if ((app_id == 0)
                            ||
                            (DP.OPMODEMANAGER.APP_IN_NEXT_MODE[i])
                        ) {
                            continue;
                        }
                        else {
                            /* TODO: If the app isn't in OFF force it off */
                        }
                    }
                }
            }

            /* Request the EPS OCP state to be changed */
            Power_request_ocp_state_for_next_opmode();

            /* Set to the next state */
            DP.OPMODEMANAGER.GRACE_TRANS_STATE 
                = OPMODEMANAGER_GRACETRANSSTATE_WAIT_OCP_CHANGE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case OPMODEMANAGER_GRACETRANSSTATE_WAIT_OCP_CHANGE:

            /* Wait for the Power app to signal OCP change success */
            if (!EventManager_poll_event(
                EVT_POWER_OPMODE_CHANGE_OCP_STATE_CHANGE_COMPLETE,
                &is_ocp_change_complete
            )) {
                DEBUG_ERR(
                    "EventManager error while polling for OCP state change"
                );
                DP.OPMODEMANAGER.ERROR_CODE = OPMODEMANAGER_ERROR_EVENTMANAGER_ERROR;
                /* If an error occured while checking for this event we should
                 * try and wait until we can actually do this check
                 * successfully, as it isn't safe to enter the next mode until
                 * the power rails are correct. */
                return false;
            }

            /* If the change is not complete */
            if (!is_ocp_change_complete) {
                return true;
            }

            /* If it is complete set the new mode and activate all the new
             * apps */
            DP.OPMODEMANAGER.OPMODE = DP.OPMODEMANAGER.NEXT_OPMODE;
            
            OpModeManager_activate_apps();

            /* Raise the opmode change event. This will trigger the TM for this
             * event to be generated. */
            if (!EventManager_raise_event(
                EVT_OPMODEMANAGER_OPMODE_CHANGED
            )) {
                DEBUG_ERR(
                    "EventManager error while raising change complete event"
                );
                DP.OPMODEMANAGER.ERROR_CODE = OPMODEMANAGER_ERROR_EVENTMANAGER_ERROR;
                /* If the event raising fails we should try to raise the TM
                 * ourselves */
                /* TODO: raise TM */
            }
            DEBUG_INF(
                "OPMODE transition to %d complete", 
                DP.OPMODEMANAGER.OPMODE
            );

            /* Set the main state into executing, the transition is done. */
            DP.OPMODEMANAGER.STATE = OPMODEMANAGER_STATE_EXECUTING;

            /* Reset the graceful transition state back to the begining */
            DP.OPMODEMANAGER.GRACE_TRANS_STATE 
                = OPMODEMANAGER_GRACETRANSSTATE_INIT;

            break;
        default:
            DEBUG_ERR(
                "Invalid DP.OPMODEMANAGER.GRACE_TRANS_STATE: %d", 
                DP.OPMODEMANAGER.GRACE_TRANS_STATE
            );
            DP.OPMODEMANAGER.ERROR_CODE 
                = OPMODEMANAGER_ERROR_INVALID_GRACE_TRANS_STATE;
            return false;
    }

    return true;
}