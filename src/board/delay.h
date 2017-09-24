/**
 * @defgroup delay
 *
 * @brief Time Delay Driver
 *
 * @ingroup drivers */

#ifndef __DELAY_H__
#define __DELAY_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @function
 * @ingroup delay
 *
 * Delays execution for a specified number of milliseconds
 *
 * @param milliseconds number of milliseconds to delay
 */
void Delay_ms(uint32_t milliseconds);

/**
 * @function
 * @ingroup delay
 *
 * Delays execution for a specified number of microseconds
 *
 * @param microseconds number of microseconds to delay
 */
void Delay_us(uint32_t microseconds);

#endif /*  __DELAY_H__ */