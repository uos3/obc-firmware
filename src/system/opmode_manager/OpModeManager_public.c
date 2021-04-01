/**
 * @file OpModeManager_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief OpModeManager public function implementations.
 * 
 * Task ref: [UT_2.9.10]
 * 
 * @version 0.1
 * @date 2021-03-08
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdbool.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/opmode_manager/OpModeManager_public.h"
#include "system/opmode_manager/OpModeManager_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool OpModeManager_init(void) {
    MemStoreManager_PersistentData pers_data;
    
    /* Check that required modules are initialised */
    if (!DP.INITIALISED ||
        !DP.EVENTMANAGER.INITIALISED ||
        !DP.MEMSTOREMANAGER.INITIALISED
    ) {
        DEBUG_ERR("Required module not initialised");
        DP.OPMODEMANAGER.ERROR_CODE = OPMODEMANAGER_ERROR_DEPENDENCY_NOT_INIT;
        return false;
    }

    /* Set the initial and next OpMode as BU. Next OpMode is set when a 
     * transition is requested, and otherwise should be the current OpMode */
    DP.OPMODEMANAGER.OPMODE = OPMODEMANAGER_OPMODE_BOOT_UP;
    DP.OPMODEMANAGER.NEXT_OPMODE = OPMODEMANAGER_OPMODE_BOOT_UP;

    /* Call first mode initialisation function */
    OpModeManager_bu_init();
    
    /* Activate the apps for the first opmode (since it's bootup this should be
     * no apps, but we do this in case an app is added in the future.) */
    OpModeManager_activate_apps();

    /* Set the state */
    DP.OPMODEMANAGER.STATE = OPMODEMANAGER_STATE_EXECUTING;

    DP.OPMODEMANAGER.INITIALISED = true;

    return true;
}

bool OpModeManager_step(void) {
    bool is_event_raised = false;
    int i, j;
    Kernel_AppId app_id;
    bool app_in_next_mode;

    if (!DP.OPMODEMANAGER.INITIALISED) {
        DEBUG_ERR("OpModeManager not initialised");
        DP.OPMODEMANAGER.ERROR_CODE = OPMODEMANAGER_ERROR_NOT_INIT;
        return false;
    }

    /* Detect OpMode change transition requests
     * 
     * This is done outside the main state machine so that the transition will
     * start this cycle, not next cycle. This is critically important for the
     * emergency transitions, but less important for graceful transitions. 
     * 
     * First check for FDIR events: */
    /* TODO: FDIR events */
    
    /* Then check for TCs: */
    if (DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE) {
        if ((DP.OPMODEMANAGER.NEXT_OPMODE == OPMODEMANAGER_OPMODE_LOW_POWER)
            ||
            (DP.OPMODEMANAGER.NEXT_OPMODE == OPMODEMANAGER_OPMODE_SAFE)
        ) {
            /* Set the state into emergency transition */
            DP.OPMODEMANAGER.STATE = OPMODEMANAGER_STATE_EMERGENCY_TRANSITION;

            DEBUG_WRN("TC requested emergency OpMode transition");
        }
        else {
            /* Set into graceful transition state */
            DP.OPMODEMANAGER.STATE = OPMODEMANAGER_STATE_GRACEFUL_TRANSITION;

            DEBUG_INF("TC requested graceful OpMode transition");
        }

        /* Unset the TC flag */
        DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE = false;
    }

    /* Finally check app requests for transitions */
    if (!OpModeManager_check_app_trans_reqs()) {
        return false;
    }

    /* If there is a transition to perform calculate whether or not each app in
     * the current mode is active in the next mode */
    if (DP.OPMODEMANAGER.OPMODE != DP.OPMODEMANAGER.NEXT_OPMODE) {
        for (i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
            /* Reset the in next mode flag for this app */
            DP.OPMODEMANAGER.APP_IN_NEXT_MODE[i] = false;

            /* Get the app id */
            app_id = CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.OPMODE][i];

            /* If the app id is 0 skip to the next one */
            if (app_id = 0) {
                continue;
            }
            else {
                /* Search for this app in the next mode */
                for (j = 0; OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++j) {
                    if (app_id 
                        == 
                        CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.NEXT_OPMODE][j]
                    ) {
                        DP.OPMODEMANAGER.APP_IN_NEXT_MODE[i] = true;
                        break;
                    }
                }
            }
        }
    }

    /* Step through the state machine */
    switch (DP.OPMODEMANAGER.STATE) {

        case OPMODEMANAGER_STATE_OFF:

            /* We should never be in this mode, we're going to issue a warning
             * and just advance to executing immediately */
            DEBUG_WRN(
                "DP.OPMODEMANAGER.STATE == OFF, but we are in step, moving to EXECUTING!"
            );
            DP.OPMODEMANAGER.STATE = OPMODEMANAGER_STATE_EXECUTING;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case OPMODEMANAGER_STATE_EXECUTING:


            /* Call all active app step functions for the current mode.
             * 
             * Note: system apps are stepped by the kernel, not OpModeManager.
             */
            if (!OpModeManager_call_active_app_steps()) {
                return false;
            }

            /* Call the mode's step function */
            switch (DP.OPMODEMANAGER.OPMODE) {
                case OPMODEMANAGER_OPMODE_BOOT_UP:
                    if (!OpModeManager_bu_step()) {
                        return false;
                    }
                    break;
                default:
                    DEBUG_WRN(
                        "No step function for mode %d", 
                        DP.OPMODEMANAGER.OPMODE
                    );
            }

            break;
        case OPMODEMANAGER_STATE_GRACEFUL_TRANSITION:

            /* Run the graceful transition substate machine */
            if (!OpModeManager_step_graceful_transition()) {
                return false;
            }

            break;
        case OPMODEMANAGER_STATE_EMERGENCY_TRANSITION:

            /* Force stop all user apps which are not active in the next mode */
            for (i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
                /* Reset the app in next mode flag */
                app_in_next_mode = false;

                /* Get the app id */
                app_id = CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.OPMODE][i];

                /* If the app id is 0 or the app is in the next mode skip to 
                 * the next one */
                if ((app_id == 0)
                    ||
                    (DP.OPMODEMANAGER.APP_IN_NEXT_MODE[i])
                ) {
                    continue;
                }
                else {
                    /* TODO: Force stop */
                }
            }

            /* Set the next power state */
            Power_request_ocp_state_for_next_opmode();

            /* Set the new OPMODE */
            DP.OPMODEMANAGER.OPMODE = DP.OPMODEMANAGER.NEXT_OPMODE;

            /* Activate any apps required in the new opmode */
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
                "OPMODE emergency transition to %d complete", 
                DP.OPMODEMANAGER.OPMODE
            );

            /* Set manager into executing */
            DP.OPMODEMANAGER.STATE = OPMODEMANAGER_STATE_EXECUTING;

            break;
        default:
            DEBUG_ERR(
                "Invalid DP.OPMODEMANAGER.STATE: %d", 
                DP.OPMODEMANAGER.STATE
            );
            DP.OPMODEMANAGER.ERROR_CODE = OPMODEMANAGER_ERROR_INVALID_STATE;
            return false;
    }
    
    return true;
}