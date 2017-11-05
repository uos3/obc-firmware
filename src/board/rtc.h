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
 * Set time on Real-Time Clock.
 *
 * @parameter uint32_t* Pointer to variable containing time value to be set.
 */
void RTC_setTime(uint32_t *time);

#endif /*  __RTC_H__ */