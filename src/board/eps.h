/**
 * @defgroup eps
 *
 * @brief EPS Hardware Driver
 *
 * @details 
 *
 * @ingroup eps
 */

#ifndef __EPS_H__
#define __EPS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @function
 * @ingroup eps
 *
 * Initialise EPS Communication
 *
 */
void EPS_init(void);

/**
 * @function
 * @ingroup eps
 *
 * Verify EPS Communication
 *
 * @returns bool True if communication is successful, False else.
 */
bool EPS_selfTest(void);

/**
 * @function
 * @ingroup eps
 *
 * Read Battery Voltage
 *
 * @parameter uint16_t* Pointer to store result.
 * @returns bool True if transaction is successful, False else.
 */

bool EPS_getBatteryVoltage(uint16_t *voltage);

#endif /* __EPS_H__ */