/**
 * @file Mission_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * 
 * Task ref: TODO
 * 
 * @version 0.1
 * @date 2021-02-26
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
#include "applications/mission/Mission_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Mission_init(void) {
    /* Check that required modules are initialised */
    if (!DP.INITIALISED ||
        !DP.EVENTMANAGER.INITIALISED ||
        !DP.MEMSTOREMANAGER.INITIALISED
    ) {
        DEBUG_ERR("Required module not initialised");
        DP.MISSION.ERROR_CODE = MISSION_ERROR_DEPENDENCY_NOT_INIT;
        return false;
    }

    DP.MISSION.OPMODE = MISSION_OPMODE_BOOT_UP;
    /* TODO: Complete init */

    DP.MISSION.INITIALISED = true;

    return true;
}

bool Mission_step(void) {
    bool is_event_raised = false;

    if (!DP.MISSION.INITIALISED) {
        DEBUG_ERR("Mission not initialised");
        DP.MISSION.ERROR_CODE = MISSION_ERROR_NOT_INIT;
        return false;
    }

    /* Check if there's an OPMODE change in progress, if so check for
     * completion */
    if (DP.MISSION.OPMODE_CHANGE_IN_PROGRESS) {
        /* Poll for power change complete event */
        if (!EventManager_poll_event(
            EVT_POWER_OPMODE_CHANGE_ACTIVITIES_COMPLETE,
            &is_event_raised
        )) {
            DEBUG_ERR("Couldn't poll the Power OPMODE change complete event");
            DP.MISSION.ERROR_CODE = MISSION_ERROR_EVENTMANAGER_ERROR;
            return false;
        }
        if (is_event_raised) {
            /* Power is the last app we will wait for a change on, so the
             * OPMODE change is complete and we can move to the next mode */
            DP.MISSION.OPMODE_CHANGE_IN_PROGRESS = false;
            DP.MISSION.OPMODE = DP.MISSION.NEXT_OPMODE;

            /* Emmit the change complete event */
            if (!EventManager_raise_event(EVT_MISSION_OPMODE_CHANGED)) {
                DEBUG_ERR("Couldn't raise the OPMODE changed event");
                DP.MISSION.ERROR_CODE = MISSION_ERROR_EVENTMANAGER_ERROR;
                return false;
            }

            DEBUG_INF("OPMODE changed to %d", DP.MISSION.OPMODE);
        }
    }
    
    return true;
}

#ifdef DEBUG_MODE
bool Mission_start_opmode_change(Mission_OpMode opmode_in) {
    if (!DP.MISSION.INITIALISED) {
        DEBUG_ERR("Mission not initialised");
        DP.MISSION.ERROR_CODE = MISSION_ERROR_NOT_INIT;
        return false;
    }

    /* If the OPMODE is the current one then don't do anything */
    if (DP.MISSION.OPMODE == opmode_in) {
        DEBUG_WRN("Requested OPMODE change to current OPMODE, ignoring");
        return true;
    }

    /* Set the next OPMODE */
    DP.MISSION.NEXT_OPMODE = opmode_in;

    /* Set the change underway flag */
    DP.MISSION.OPMODE_CHANGE_IN_PROGRESS = true;

    /* Emmit the change started event */
    if (!EventManager_raise_event(EVT_MISSION_OPMODE_CHANGE_STARTED)) {
        DEBUG_ERR("Couldn't raise the OPMODE change started event");
        DP.MISSION.ERROR_CODE = MISSION_ERROR_EVENTMANAGER_ERROR;
        return false;
    }

    return true;
}
#endif