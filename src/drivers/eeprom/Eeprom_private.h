/**
 * @ingroup eeprom
 * @file Eeprom_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Private header for the EEPROM driver.
 * 
 * Task ref: [UT_2.8.6]
 * 
 * @version 0.1
 * @date 2020-12-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef H_EEPROM_PRIVATE_H
#define H_EEPROM_PRIVATE_H

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The size of the EEPROM on the TM4C123GH6PM in bytes (2KB).
 */
#define EEPROM_SIZE_BYTES (0x800)

/**
 * @brief Number of checks to perform to see if the EEPROM's state machine is
 * ready to perform an operation.
 * 
 * Note: this value is arbitrary.
 */
#define EEPROM_NUM_OP_READY_CHECKS (10)

#ifdef TARGET_UNIX
/**
 * @brief File path to the dummy EEPROM file on linux.
 */
#define EEPROM_DUMMY_FILE_PATH ("builds/linux_dummy_eeprom.bin")
#endif

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Reads the EEPROM status to determine if an operation is ready to be
 * performed. 
 * 
 * @return ErrorCode ERROR_NONE on success, EEPROM_ERROR_x on failure.
 */
ErrorCode Eeprom_op_ready(void);

#endif /* H_EEPROM_PRIVATE_H */