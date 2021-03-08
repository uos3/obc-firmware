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

    DP.OPMODEMANAGER.OPMODE = OPMODEMANAGER_OPMODE_BOOT_UP;
    /* TODO: Complete init */

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

    /* Check if there's an OPMODE change in progress, if so check for
     * completion */
    if (DP.OPMODEMANAGER.OPMODE_CHANGE_IN_PROGRESS) {
        /* Poll for power change complete event */
        if (!EventManager_poll_event(
            EVT_POWER_OPMODE_CHANGE_ACTIVITIES_COMPLETE,
            &is_event_raised
        )) {
            DEBUG_ERR("Couldn't poll the Power OPMODE change complete event");
            DP.OPMODEMANAGER.ERROR_CODE = OPMODEMANAGER_ERROR_EVENTMANAGER_ERROR;
            return false;
        }
        if (is_event_raised) {
            /* Power is the last app we will wait for a change on, so the
             * OPMODE change is complete and we can move to the next mode */
            DP.OPMODEMANAGER.OPMODE_CHANGE_IN_PROGRESS = false;
            DP.OPMODEMANAGER.OPMODE = DP.OPMODEMANAGER.NEXT_OPMODE;

            /* Emmit the change complete event */
            if (!EventManager_raise_event(EVT_OPMODEMANAGER_OPMODE_CHANGED)) {
                DEBUG_ERR("Couldn't raise the OPMODE changed event");
                DP.OPMODEMANAGER.ERROR_CODE = OPMODEMANAGER_ERROR_EVENTMANAGER_ERROR;
                return false;
            }

            DEBUG_INF("OPMODE changed to %d", DP.OPMODEMANAGER.OPMODE);
        }
    }
    
    return true;
}