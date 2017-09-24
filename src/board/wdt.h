/**
 * @defgroup wdt
 *
 * @brief External watchdog driver
 *
 * @details Driver for kicking the external window watchdog device.
 *
 * @ingroup drivers
 */

#ifndef __WDT_H__
#define __WDT_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @function
 * @ingroup wdt
 *
 * Kicks the external window watchdog device.
 */
void WDT_kick(void);

#endif /*  __LED_H__ */