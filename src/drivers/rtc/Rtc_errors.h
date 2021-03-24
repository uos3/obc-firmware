/**
 * @file Rtc_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Errors that can occur in the Rtc module.
 * 
 * Task ref: [UT_2.8.4]
 * 
 * @version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_RTC_ERRORS_H
#define H_RTC_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_public.h"

/* -------------------------------------------------------------------------   
 * ERRORS
 * ------------------------------------------------------------------------- */

/**
 * @brief Indicates that the hibernation module wasn't enabled in the maximum
 * number of checks.
 */
#define RTC_ERROR_PERIPHERAL_NOT_INIT ((ErrorCode)(MOD_ID_RTC | 1))

/**
 * @brief A timestamp is in the past, when it was expected to be in the future.
 */
#define RTC_ERROR_TIME_IN_PAST ((ErrorCode)(MOD_ID_RTC | 2))

#endif /* H_RTC_ERRORS_H */