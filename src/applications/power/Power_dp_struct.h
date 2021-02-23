/**
 * @file Power_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application DataPool struct definition
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_POWER_DP_STRUCT_H
#define H_POWER_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdbool.h>

/* Internal includes */
#include "system/kernel/Kernel_errors.h"
#include "system/event_manager/EventManager_public.h"
#include "applications/power/Power_public.h"
#include "components/eps/Eps_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Power Application DataPool structure.
 */
typedef struct _Power_Dp {
    
    /**
     * @brief Indicates if the Power app is initialised (true) or not (false).
     * 
     * @dp 1
     */
    bool INITIALISED;

    /**
     * @brief Stores error codes that occur during operation.
     * 
     * @dp 2
     */
    ErrorCode ERROR_CODE;

    /**
     * @brief Stores error codes returned by the Timer driver.
     * 
     * @dp 3
     */
    ErrorCode TIMER_ERROR_CODE;

    /**
     * @brief Flag indicating whether or not Mission is requested to transition
     * into Low Power (LP) OpMode. 
     * 
     * This flag will be set if the Power app detects an unsuitably low battery
     * voltage, which means that the Mission app must transition to LP mode.
     * 
     * The priority of this transition is less than that of Safe mode, meaning
     * that if a safe mode transition should occur at the same time an LP
     * transition should happen, the Mission app should transition to Safe.
     * 
     * @dp 4
     */
    bool LOW_POWER_MODE_REQUEST;

    /**
     * @brief The event associated with the app's primary task timer.
     * 
     * @dp 5
     */
    Event TASK_TIMER_EVENT;

    /**
     * @brief The requested state of the OCP rails, which is based on the
     * OpMode and the stored OpMode-OCP state configuration vector.
     * 
     * @dp 6
     */
    Eps_OcpState REQUESTED_OCP_STATE;

} Power_Dp;

#endif /* H_POWER_DP_STRUCT_H */