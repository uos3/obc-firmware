/**
 * @defgroup fram
 *
 * @brief FRAM Functions & Definitions
 *
 * @details 
 *
 * @ingroup fram
 */

#ifndef __FRAM_H__
#define __FRAM_H__

#include "../firmware.h"

/**
 * @function
 * @ingroup fram
 *
 * Initialise FRAM Device
 *
 */
void Fram_init(void);

/**
 * @function
 * @ingroup fram
 *
 * Check FRAM Device returns correct Device ID, indicating likelihood of correct operation
 *
 * @returns bool True if Device ID is return correctly, False else.
 */
bool FRAM_selfTest(void);

void FRAM_write(uint32_t address, uint8_t *data, uint32_t length);

void FRAM_read(uint32_t address, uint8_t *data, uint32_t length);

#endif /* __FRAM_H__ */