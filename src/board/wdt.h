/** @file wdt.h
 *
 * External watchdog driver.
 *
 * @ingroup drivers
 * @{
 */

#ifndef __WDT_H__
#define __WDT_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * Kicks the external window watchdog device.
 */
void WDT_kick(void);

#endif /*  __LED_H__ */

/**
 * @}
 */