/**
 * @ingroup eeprom
 * @file Eeprom_private_tm4c.c
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

ErrorCode Eeprom_op_ready(void) {
    /* EEPROM return code */
    uint32_t eeprom_rc = 0;

    /* Loop until the EEPROM is ready */
    for (int i = 0; i < EEPROM_NUM_OP_READY_CHECKS; ++i) {

        /* Get the status of the EEPROM */
        eeprom_rc = EEPROMStatusGet();

        /* If the EEPROM is ready */
        if (eeprom_rc == 0) {
            break;
        }
    }

    /* If the system is waiting on an operation RC_WORKING or RC_WRUSY we group
     * these into the EEPROM_ERROR_BUSY error */
    if (
        ((eeprom_rc & EEPROM_RC_WORKING) == EEPROM_RC_WORKING)
        || 
        ((eeprom_rc & EEPROM_RC_WRBUSY) == EEPROM_RC_WRBUSY)
    ) {
        DEBUG_ERR("EEPROM status is WORKING or WRBUSY");
        return EEPROM_ERROR_BUSY;
    }
    else if ((eeprom_rc & EEPROM_RC_NOPERM) == EEPROM_RC_NOPERM) {
        DEBUG_ERR(
            "EEPROM cannot write as destination permissions do not allow it."
        );
        return EEPROM_ERROR_NO_PERMISSIONS;
    }
    /* All other values are non-errors */
    else {
        return ERROR_NONE;
    }
}