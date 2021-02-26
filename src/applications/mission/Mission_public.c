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
    
    /* TODO */
    
    return true;
}