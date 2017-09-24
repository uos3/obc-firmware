/** @file random.h
 *
 * Random driver.
 *
 * @author Phil Crump
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdint.h>
#include <stdbool.h>

uint32_t Random(uint32_t max);

#endif /*  __RANDOM_H__ */

/**
 * @}
 */