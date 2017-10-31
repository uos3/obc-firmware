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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* 1Mbit (128KB) FM25V10 */
#define FRAM_SIZE	0x20000
/* 4Mbit (512KB) CY15B104Q */
//#define FRAM_SIZE	0x80000

#define FRAM_MAX_ADDRESS (FRAM_SIZE - 1)

/**
 * @function
 * @ingroup fram
 *
 * Initialise FRAM Device
 *
 */
void FRAM_init(void);

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