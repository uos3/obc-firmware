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

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Power_init(void) {
    
    /* Check that required modules are initialised */
    if (!DP.INITIALISED ||
        !DP.EVENTMANAGER.INITIALISED ||
        !DP.MISSION.INITIALISED ||
        !DP.EPS.INITIALISED
    ) {
        DEBUG_ERR("Required module not initialised");
        DP.POWER.ERROR_CODE = POWER_ERROR_DEPENDENCY_NOT_INIT;
        return false;
    }

    return true;
}

bool Power_step(void) {
    ErrorCode error;

    /* The power control app runs in all modes, so we're going to start the
     * task timer now, rather than checking for the correct mode */
    error = Timer_start_periodic(
        (double)CFG.power_task_timer_duration_s,
        &DP.POWER.TASK_TIMER_EVENT
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("Couldn't start Power task timer");
        DP.POWER.ERROR_CODE = POWER_ERROR_TASK_TIMER_NOT_STARTED;
        DP.POWER.TIMER_ERROR_CODE = error;
        return false;
    }

    return true;
}