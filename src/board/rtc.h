/**
 * @defgroup rtc
 *
 * @brief Real-Time Clock Driver
 *
 * @details Real-Time Clock Driver.
 *
 * @ingroup drivers
 */

#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @function
 * @ingroup rtc
 *
 * Initialise Real-Time Clock.
 * Will block for ~1500ms if RTC not already initialised.
 *
 */
void RTC_init(void);

/**
 * @function
 * @ingroup rtc
 *
 * Get time from Real-Time Clock.
 *
 * @parameter uint32_t* Pointer to variable to store time value.
 */
void RTC_getTime(uint32_t *time);

/**
 * @function
 * @ingroup rtc
 *
 * Get time in milliseconds from Real-Time Clock.
 *
 * @parameter uint64_t* Pointer to variable to store time in milliseconds value.
 */
void RTC_getTime_ms(uint64_t *time_ms);

/**
 * @function
 * @ingroup rtc
 *
 * Set time on Real-Time Clock.
 *
 * @parameter uint32_t* Pointer to variable containing time value to be set.
 */
void RTC_setTime(uint32_t *time);

/**
 * @function
 * @ingroup rtc
 *
 * Timer function, returns true if current_time < start_time_ms or current_time > start_time_ms + period. 
 *
 * @parameter uint64_t Start time of timer
 * @parameter uint64_t Period of timer
 */
bool RTC_timerElapsed_ms(uint64_t start_time_ms, uint64_t period_ms);

#endif /*  __RTC_H__ */