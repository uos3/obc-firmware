/**
 * @defgroup eeprom
 *
 * @brief EEPROM Functions & Definitions
 *
 * @details 
 *
 * @ingroup eeprom
 */

#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* 2KB TM4C123GH6PM */
#define EEPROM_SIZE	0x800

#define EEPROM_MAX_ADDRESS (EEPROM_SIZE - 1)

//#define EEPROM_MISSION_DATA_ADDRESS 0x00000001
//#define EEPROM_ADM_DEPLOY_ATTEMPTS 0x00000002
//#define EEPROM_CONFIGURATION_DATA_ADDRESS 0x0100

/**
 * @function
 * @ingroup eeprom
 *
 * Initialise EEPROM Device
 *
 */
bool EEPROM_init(void);

/**
 * @function
 * @ingroup eeprom
 *
 * Check EEPROM returns correct size, indicating likelihood of correct operation
 *
 * @returns bool True if size is returned correctly, False else.
 */
bool EEPROM_selfTest(void);

/**
 * @function
 * @ingroup eeprom
 *
 * Write data to EEPROM, beginning at specified memory address.
 *
 * @parameter uint32_t Memory address to begin writing data.
 * @parameter uint32_t* Pointer to data buffer.
 * @parameter uint32_t Length to write in bytes.
 */
void EEPROM_write(uint32_t address, uint32_t *data, uint32_t length);

/**
 * @function
 * @ingroup eeprom
 *
 * Read data from EEPROM, beginning at specified memory address.
 *
 * @parameter uint32_t Memory address to begin reading data.
 * @parameter uint32_t* Pointer to data buffer.
 * @parameter uint32_t Length to read in bytes.
 */
void EEPROM_read(uint32_t address, uint32_t *data, uint32_t length);

#endif /* __EEPROM_H__ */