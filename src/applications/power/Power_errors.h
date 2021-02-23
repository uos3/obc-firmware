 /**
 * @file Power_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application error code definitions
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_POWER_ERRORS_H
#define H_POWER_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief A required dependent module (DataPool, EventManager, Mission, Eps)
 * was not initialised.
 */
#define POWER_ERROR_DEPENDENCY_NOT_INIT ((ErrorCode)(MOD_ID_POWER | 1))

/**
 * @brief The app's primary task timer couldn't be started due to a timer
 * error. See DP.POWER.TIMER_ERROR_CODE for the root cause.
 */
#define POWER_ERROR_TASK_TIMER_NOT_STARTED ((ErrorCode)(MOD_ID_POWER | 2))

#endif /* H_POWER_ERRORS_H */