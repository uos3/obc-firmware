/**
 * @file Delay_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Public definition of the Delay module.
 * 
 * Task ref: [UT_2.8.3]
 * 
 * The Delay module is an _INPRECISE_ timing strategy which provides simple
 * number-of-cycles based delays to the software. High level code should _NOT_
 * be using this module, it is made available for other low-level drivers only.
 * 
 * If you need precise timing either use the Rtc module or use a dedicated
 * timer from the Timer driver.
 * 
 * @version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_DELAY_PUBLIC_H
#define H_DELAY_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Number of microseconds in a millisecond. 
 */
#define DELAY_MICROS_TO_MILLIS (1000)

/**
 * @brief The empirically found number of delay cycles per microsecond.
 *
 * Found by Phil Crump on 25-06-2017, commit
 * ffc9adf5b931de9cfc6a46c23328ea01f1f2fd53. 
 */
#define DELAY_CYCLES_PER_MICRO (13)

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Delay for the given number of milliseconds.
 * 
 * _WARNING_: This is not guarenteed to be precise, and will lock up the
 * processor for the given duration. USE WITH CAUTION.
 * 
 * Prefer using the Rtc or Timer modules over this function.
 * 
 * @param milliseconds_in Number of milliseconds to delay for, up to 330382 ms.
 */
void Delay_ms(uint32_t milliseconds_in);

/**
 * @brief Delay for the given number of microseconds.
 * 
 * _WARNING_: This is not guarenteed to be precise, and will lock up the
 * processor for the given duration. USE WITH CAUTION.
 * 
 * Prefer using the Rtc or Timer modules over this function.
 * 
 * @param microseconds_in Number of milliseconds to delay for, up to 
 * 330382099 us.
 */
void Delay_us(uint32_t microseconds_in);

#endif /* H_DELAY_PUBLIC_H */