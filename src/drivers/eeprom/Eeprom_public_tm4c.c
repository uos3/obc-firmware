/**
 * @ingroup eeprom
 * @file Eeprom_public_tm4c.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Implementation of the EEPROM driver for the TM4C.
 * 
 * Task ref: [UT_2.8.6]
 * 
 * @version 0.1
 * @date 2020-12-08
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdbool.h>
#include <stdint.h>

/* External includes */
#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "drivers/eeprom/Eeprom_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Eeprom_init(void) {
    /* Enable the peripheral */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

    /* Wait for the peripheral to be ready */
    bool periph_enabled = false;
    for (int i = 0; i < BOARD_NUM_PERIPH_ENABLED_CHECKS; ++i) {
        if (SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0)) {
            periph_enabled = true;
            break;
        }
    }

    /* If the peripheral isn't enabled retun an error */
    if (!periph_enabled) {
        DEBUG_ERR(
            "EEPROM periphal could not be enabled: 0x%04X",
            EEPROM_ERROR_INIT_RECOVERY_FAILED
        );
        return EEPROM_ERROR_PERIP_NOT_READY;
    }

    /* If the peripheral was enabled then call the EEPROM init function */
    if (EEPROMInit() != EEPROM_INIT_OK) {
        DEBUG_ERR(
            "EEPROM recovery action failed: 0x%04X", 
            EEPROM_ERROR_INIT_RECOVERY_FAILED
        );
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    return ERROR_NONE;
}

ErrorCode Eeprom_self_test(void) {
    /* Get the size of the EEPROM reported by Tivaware */
    uint32_t eeprom_size = EEPROMSizeGet();
    
    /* If the size doesn't match the expected size return an error */
    if (eeprom_size != EEPROM_SIZE_BYTES) {
        DEBUG_ERR(
            "EEPROM self test failed, expected EEPROM to be 0x%04X bytes long but got 0x%04X instead",
            EEPROM_SIZE_BYTES,
            eeprom_size
        );

        return EEPROM_ERROR_UNEXPECTED_SIZE;
    }

    return ERROR_NONE;
}

ErrorCode Eeprom_write(
    uint32_t address_in, 
    uint32_t *p_data_in, 
    uint32_t length_in
) {
    /* Error code for EEPROM calls */
    ErrorCode eeprom_error = ERROR_NONE;
    /* Return code of the EEPROM Tivaware calls */
    uint32_t eeprom_rc = 0;

    /* Check that the address and length combination will fit in the EEPROM */
    if (address_in + length_in >= EEPROM_SIZE_BYTES) {
        DEBUG_ERR(
            "EEPROM cannot write %d bytes to address 0x%04X - EEPROM too small",
            length_in,
            address_in  
        );
        return EEPROM_ERROR_WRITE_TOO_LARGE;
    }

    /* Check that the length and address are mutliples of 4 */
    if (address_in % 4 != 0
        ||
        length_in % 4 != 0
    ) {
        DEBUG_ERR("EEPROM write inputs not word-aligned");
        return EEPROM_ERROR_INPUTS_NOT_WORD_ALIGNED;
    }
    
    /* Check if the EEPROM is ready to perform an action */
    eeprom_error = Eeprom_op_ready();
    if (eeprom_error != ERROR_NONE) {
        DEBUG_ERR(
            "Cannot write to EEPROM as EEPROM is not ready to perform an operation"
        );
        return eeprom_error;
    }

    /* Write the data to the EEPROM */
    eeprom_rc = EEPROMProgram(p_data_in, address_in, length_in);

    /* Check that the NOPERM bit isn't set */
    if (eeprom_rc & EEPROM_RC_NOPERM == 1) {
        DEBUG_ERR(
            "Invalid permissions to write to the EEPROM address 0x%04X",
            address_in    
        );
        return EEPROM_ERROR_NO_PERMISSIONS;
    }

    return ERROR_NONE;
}

ErrorCode Eeprom_read(
    uint32_t address_in, 
    uint32_t *p_data_out, 
    uint32_t length_in
) {
    /* Error code for EEPROM calls */
    ErrorCode eeprom_error = ERROR_NONE;

    /* Check that the address and length combination will fit in the EEPROM */
    if (address_in + length_in >= EEPROM_SIZE_BYTES) {
        DEBUG_ERR(
            "EEPROM cannot read %d bytes from address 0x%04X - EEPROM too small",
            length_in,
            address_in  
        );
        return EEPROM_ERROR_READ_TOO_LARGE;
    }

    /* Check that the length and address are mutliples of 4 */
    if (address_in % 4 != 0
        ||
        length_in % 4 != 0
    ) {
        DEBUG_ERR("EEPROM write inputs not word-aligned");
        return EEPROM_ERROR_INPUTS_NOT_WORD_ALIGNED;
    }
    
    /* Check if the EEPROM is ready to perform an action */
    eeprom_error = Eeprom_op_ready();
    if (eeprom_error != ERROR_NONE) {
        DEBUG_ERR(
            "Cannot read from EEPROM as EEPROM is not ready to perform an operation"
        );
        return eeprom_error;
    }

    /* Read the data from the EEPROM */
    EEPROMRead(p_data_out, address_in, length_in);

    return ERROR_NONE;
}
