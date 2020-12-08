/**
 * @ingroup drivers
 * @file Eeprom_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Driver for the TM4C's EEPROM (non-volatile on-chip memory).
 * 
 * Task ref: [UT_2.8.6]
 * 
 * The EEPROM is the on-chip non-volatile Electrically Erasable Programable
 * Read Only Memory module of the TM4C, which provides easy-to-use persitant
 * data storage.
 * 
 * Eeprom is a system driver, and hense uses a blocking interface, as apposed
 * to the IO drivers. 
 * 
 * @version 0.1
 * @date 2020-12-08
 * 
 * @copyright Copyright (c) UoS3 2020
 * 
 * @defgroup eeprom EEPROM
 * @{
 */

#ifndef H_EEPROM_PUBLIC_H
#define H_EEPROM_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* Internal includes */
#include "drivers/eeprom/Eeprom_errors.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the Eeprom module.
 * 
 * This function enables the EEPROM peripheral and initiases it. Recovery
 * action neccessary in case of a power failure ruing an EEPROM write is
 * attempted. If this recovery fails EEPROM_ERROR_INIT_RECOVERY_FAILED will be
 * returned. 
 * 
 * In addition EEPROM_ERROR_PERIP_NOT_READY may be returned if the ready check
 * on the EEPROM peripheral fails.
 * 
 * This function MUST be called before using the EEPROM. Permanent data loss
 * may result if this is not done.
 * 
 * @return ErrorCode ERROR_NONE on success, EEPROM_ERROR_x on failure.
 */
ErrorCode Eeprom_init(void);

/**
 * @brief Perform the self test on the EEPROM peripheral.
 * 
 * This function will read the size of the EEPROM and check it against a
 * pre-defined value EEPROM_SIZE_BYTES. If this check fails
 * EEPROM_ERROR_UNEXPECTED_SIZE will be returned.
 * 
 * @return ErrorCode ERROR_NONE on success, EEPROM_ERROR_x on failure.
 */
ErrorCode Eeprom_self_test(void);

/**
 * @brief Write an array of words into the EEPROM at the given address.
 * 
 * Note: The EEPROM works on word-length units (32 bits, 4 bytes), not bytes.
 * 
 * @param address_in The word-aligned byte address to write to. This value must
 *        be a multiple of 4.
 * @param p_data_in Pointer to the first word of data to write.
 * @param length_in The number of bytes to write. This value must be a multiple
 *        of 4 as the EEPROM is word-aligned.
 * @return ErrorCode ERROR_NONE on success, EEPROM_ERROR_x on failure.
 */
ErrorCode Eeprom_write(
    uint32_t address_in, 
    uint32_t *p_data_in, 
    uint32_t length_in
);

/**
 * @brief Read an array of words from the EEPROM at the given address.
 * 
 * Note: The EEPROM works on word-length units (32 bits, 4 bytes), not bytes.
 * 
 * @param address_in The word-aligned byte address to read from. This value 
 *        must be a multiple of 4.
 * @param p_data_out Pointer to the buffer to store the data in once read. The
 *        user is responsible for ensuring this buffer is at least length_in 
 *        bytes long.
 * @param length_in The number of bytes to read. This value must be a multiple
 *        of 4 as the EEPROM is word-aligned.
 * @return ErrorCode ERROR_NONE on success, EEPROM_ERROR_x on failure.
 */
ErrorCode Eeprom_read(
    uint32_t address_in, 
    uint32_t *p_data_out, 
    uint32_t length_in
);

#endif /* H_EEPROM_PUBLIC_H */