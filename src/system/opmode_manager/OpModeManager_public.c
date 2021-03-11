/**
 * @file OpModeManager_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
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
    /* Check that required modules are initialised */
    if (!DP.INITIALISED ||
        !DP.EVENTMANAGER.INITIALISED ||
        !DP.MEMSTOREMANAGER.INITIALISED
    ) {
        DEBUG_ERR("Required module not initialised");
        DP.OPMODEMANAGER.ERROR_CODE = OPMODEMANAGER_ERROR_DEPENDENCY_NOT_INIT;
        return false;
    }

    /* Set the initial OpMode */
    DP.OPMODEMANAGER.OPMODE = OPMODEMANAGER_OPMODE_BOOT_UP;
    
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
    }

    /* Finally check app requests for transitions */
    if (!OpModeManager_check_app_trans_reqs()) {
        return false;
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

            break;
        case OPMODEMANAGER_STATE_GRACEFUL_TRANSITION:

            break;
        case OPMODEMANAGER_STATE_EMERGENCY_TRANSITION:

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