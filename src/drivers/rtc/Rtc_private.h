/**
 * @file Rtc_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Private definitions for the Rtc module.
 * 
 * Task ref: [UT_2.8.4]
 * 
 * @version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_RTC_PRIVATE_H
#define H_RTC_PRIVATE_H

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Maximum number of checks to be made on the HIB peripheral being
 * enabled.
 */
#define RTC_MAX_NUM_PERIPH_READY_CHECKS (10)

/**
 * @brief Number of milliseconds in a second.
 */
#define RTC_SEC_TO_MILLISEC (1000)

/**
 * @brief Number of subseconds in a second.
 * 
 */
#define RTC_SEC_TO_SUBSEC (32768)

/**
 * @brief Delay used to allow the oscillator to stabilise.
 * 
 * TODO: Orignally set by Phil Crump in commit
 * 06079be325b10500568529c5a6756e2594f72bc4, source of this number is unclear.
 */
#define RTC_OSCILLATOR_STABILISE_DELAY_MS (1500)

#endif /* H_RTC_PRIVATE_H */