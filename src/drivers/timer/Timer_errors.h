/**
 * @file Timer_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Defines error codes for the Timer driver.
 * 
 * Task ref: [UT_2.8.1]
 * 
 * @version 0.1
 * @date 2021-02-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef H_TIMER_ERRORS_H
#define H_TIMER_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * ERRORS
 * ------------------------------------------------------------------------- */

/**
 * @brief Indicates that a timer peripheral couldn't be enabled.
 */
#define TIMER_ERROR_PERIPH_ENABLE_FAILED ((ErrorCode)(MOD_ID_TIMER | 1))

/**
 * @brief Raised if attempting to start a timer with a duration longer than is
 * capable for the hardware (i.e. longer than 255*clock_frequency*UINT64_MAX).
 */
#define TIMER_ERROR_DURATION_TOO_LARGE ((ErrorCode)(MOD_ID_TIMER | 2))

/**
 * @brief Raised if no valid integer factor of the required timer cycle count
 * was found. Valid factors are those which get within 1 of the target number
 * of cycles.
 */
#define TIMER_ERROR_NO_FACTORS_FOUND ((ErrorCode)(MOD_ID_TIMER | 3))

/**
 * @brief Raised if a timer pointer was NULL.
 */
#define TIMER_ERROR_NULL_TIMER ((ErrorCode)(MOD_ID_TIMER | 4))

/**
 * @brief Raised if attempting to start a new 16 bit timer but none are
 * available. 
 */
#define TIMER_ERROR_NO_16_BIT_TIMERS ((ErrorCode)(MOD_ID_TIMER | 5))

/**
 * @brief Raised if attempting to start a new 32 bit timer but none are
 * available. 
 */
#define TIMER_ERROR_NO_32_BIT_TIMERS ((ErrorCode)(MOD_ID_TIMER | 6))

/**
 * @brief Raised if attempting to start a new 64 bit timer but none are
 * available. 
 */
#define TIMER_ERROR_NO_64_BIT_TIMERS ((ErrorCode)(MOD_ID_TIMER | 7))

#endif /* H_TIMER_ERRORS_H */