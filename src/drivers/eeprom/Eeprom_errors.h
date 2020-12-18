/**
 * @ingroup eeprom
 * @file Eeprom_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Defines error codes for the Eeprom module.
 * 
 * Task ref: [UT_2.8.6]
 * 
 * @version 0.1
 * @date 2020-12-08
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_EEPROM_ERRORS_H
#define H_EEPROM_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Error indicating that recovery action required by the EEPROM
 * initialisation failed.
 * 
 * From the Tivaware user manual section 9.2.3.8 if this error occurs in a
 * stable power state this is an indicator that the EEPROM has exceeded its
 * lifetime of write/erases. If the supply voltage is unstable when this error
 * occurs the operation may still succeed once the voltage is stabilised.
 */
#define EEPROM_ERROR_INIT_RECOVERY_FAILED ((ErrorCode)(MOD_ID_EEPROM | 1))

/**
 * @brief Indicates that the maximum number of wait attempts on the EEPROM
 * peripheral being enabled has been exceeded.
 */
#define EEPROM_ERROR_PERIP_NOT_READY ((ErrorCode)(MOD_ID_EEPROM | 2))

/**
 * @brief The EEPROM self test failed because the size returned by the Tivaware
 * function EEPROMSizeGet() and the expected size EEPROM_SIZE_BYTES differ.
 */
#define EEPROM_ERROR_UNEXPECTED_SIZE ((ErrorCode)(MOD_ID_EEPROM | 3))

/**
 * @brief The EEPROM status is either EEPROM_RC_WORKING or EEPROM_RC_WRBUSY,
 * meaning that a write operation cannot be done until the EEPROM is no longer
 * busy. 
 */
#define EEPROM_ERROR_BUSY ((ErrorCode)(MOD_ID_EEPROM | 4))

/**
 * @brief A write operation cannot succeed because the user has no permission
 * to write to the address (EEPROM_RC_NOPERM). 
 */
#define EEPROM_ERROR_NO_PERMISSIONS ((ErrorCode)(MOD_ID_EEPROM | 5))

/**
 * @brief An EEPROM write failed because the combintion of address and length
 * would exceed the size of the EEPROM.
 */
#define EEPROM_ERROR_WRITE_TOO_LARGE ((ErrorCode)(MOD_ID_EEPROM | 6))

/**
 * @brief An EEPROM read failed because the combintion of address and length
 * would exceed the size of the EEPROM.
 */
#define EEPROM_ERROR_READ_TOO_LARGE ((ErrorCode)(MOD_ID_EEPROM | 7))

/**
 * @brief Inputs to an EEPROM function must be word-aligned, but some inputs
 * wern't.
 */
#define EEPROM_ERROR_INPUTS_NOT_WORD_ALIGNED ((ErrorCode)(MOD_ID_EEPROM | 8))

#endif /* H_EEPROM_ERRORS_H */