/** @file random.h
 *
 * Random driver.
 *
 * @ingroup drivers
 * @{
 */

#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Returns an unsigned random integer of less than or equal to the given parameter.
 *
 * @param max maximum output value
 * @returns unsigned integer less than or equal to 'max'
 */
uint32_t Random(uint32_t max);

#endif /*  __RANDOM_H__ */

/**
 * @}
 */