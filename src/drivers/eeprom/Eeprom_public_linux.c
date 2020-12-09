/**
 * @ingroup eeprom
 * @file Eeprom_public_linux.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Implementation of the EEPROM driver for Linux.
 * 
 * Task ref: [UT_2.8.6]
 * 
 * On linux the EEPROM is modeled by a file stored next to the executable run
 * named linux_dummy_eeprom.bin. This file is used to simulate persistant 
 * storage that EEPROM provides. It is a raw binary array which is read and 
 * written in bulk so as not to overcomplicate this dummy driver.
 * 
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "drivers/eeprom/Eeprom_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Eeprom_init(void) {
    /* Use stat to check if the file exists */
    struct stat stat_buff;
    bool file_exists = (bool)(stat(EEPROM_DUMMY_FILE_PATH, &stat_buff) == 0);

    /* If the file doesn't exist need to create it with the proper init value
     * of 0xff for all bytes, which is what the mass erase leaves the EEPROM at
     * according to the Tivaware manual. */
    if (!file_exists) {
        /* Attempt to create the EEPROM file */
        FILE *fp_eeprom = fopen(EEPROM_DUMMY_FILE_PATH, "wb");

        /* Check for success */
        if (fp_eeprom == NULL) {
            DEBUG_ERR("Could not create dummy EEPROM file");
            return EEPROM_ERROR_INIT_RECOVERY_FAILED;
        }

        /* Create dummy array */
        uint8_t dummy[EEPROM_SIZE_BYTES];
        memset((void *)dummy, 0xff, sizeof(dummy));

        /* Write the data into the file */
        size_t num_items = fwrite(dummy, sizeof(dummy), 1, fp_eeprom);

        /* Check for success */
        if (num_items != 1) {
            DEBUG_ERR("Could not create new empty EEPROM");
            return EEPROM_ERROR_INIT_RECOVERY_FAILED;
        }

        /* Close the file */
        if (fclose(fp_eeprom) != 0) {
            DEBUG_ERR("Could not close EEPROM dummy file");
            return EEPROM_ERROR_INIT_RECOVERY_FAILED;
        }

        DEBUG_TRC(
            "Wrote dummy EEPROM of size %d bytes to %s",
            EEPROM_SIZE_BYTES,
            EEPROM_DUMMY_FILE_PATH
        );
    }

    return ERROR_NONE;
}

ErrorCode Eeprom_self_test(void) {
    /* Use stat to check if the file exists */
    struct stat stat_buff;
    bool file_exists = (bool)(stat(EEPROM_DUMMY_FILE_PATH, &stat_buff) == 0);

    /* If the eeprom doesn't exist give init error */
    if (!file_exists) {
        DEBUG_ERR("EEPROM dummy doesn't exist, did you forget to Eeprom_init()?");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    /* Check the stat buffer to see the size is equal to the size defined in
     * the header */
    if (stat_buff.st_size != EEPROM_SIZE_BYTES) {
        DEBUG_ERR(
            "Dummy EEPROM file is %d bytes long, expected %d", 
            stat_buff.st_size,
            EEPROM_SIZE_BYTES
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
    /* Use stat to check if the file exists */
    struct stat stat_buff;
    bool file_exists = (bool)(stat(EEPROM_DUMMY_FILE_PATH, &stat_buff) == 0);

    /* If the eeprom doesn't exist give init error */
    if (!file_exists) {
        DEBUG_ERR("EEPROM dummy doesn't exist, did you forget to Eeprom_init()?");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    /* Check that the address and length combination will fit in the EEPROM */
    if (address_in + length_in >= EEPROM_SIZE_BYTES) {
        DEBUG_ERR(
            "EEPROM cannot write %d bytes to address 0x%04X - EEPROM too small",
            length_in,
            address_in  
        );
        return EEPROM_ERROR_WRITE_TOO_LARGE;
    }

    /* Attempt to open the EEPROM file */
    FILE *fp_eeprom = fopen(EEPROM_DUMMY_FILE_PATH, "r+b");

    /* Check for success */
    if (fp_eeprom == NULL) {
        DEBUG_ERR("Could not open dummy EEPROM file");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    /* Read the file in */
    uint8_t dummy[EEPROM_SIZE_BYTES] = {0xff};
    size_t num_items = fread(dummy, sizeof(dummy), 1, fp_eeprom);

    /* Check for success */
    if (num_items != 1) {
        DEBUG_ERR("Could not read EEPROM file");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    /* Change the bytes in dummy */
    memcpy((void *)&dummy[address_in], (void *)p_data_in, length_in);

    /* Rewind the file to the begining, so that the write will overwrite data
     * in the file rather than append */       
    rewind(fp_eeprom);

    /* Write the dummy back out to the file */
    num_items = fwrite(dummy, sizeof(dummy), 1, fp_eeprom);

    /* Check for success */
    if (num_items != 1) {
        DEBUG_ERR("Error writing to dummy EEPROM file");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    /* Close the file */
    if (fclose(fp_eeprom) != 0) {
        DEBUG_ERR("Could not close EEPROM dummy file");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    return ERROR_NONE;
}

ErrorCode Eeprom_read(
    uint32_t address_in, 
    uint32_t *p_data_out, 
    uint32_t length_in
) {
    /* Use stat to check if the file exists */
    struct stat stat_buff;
    bool file_exists = (bool)(stat(EEPROM_DUMMY_FILE_PATH, &stat_buff) == 0);

    /* If the eeprom doesn't exist give init error */
    if (!file_exists) {
        DEBUG_ERR("EEPROM dummy doesn't exist, did you forget to Eeprom_init()?");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    /* Check that the address and length combination will fit in the EEPROM */
    if (address_in + length_in >= EEPROM_SIZE_BYTES) {
        DEBUG_ERR(
            "EEPROM cannot read %d bytes from address 0x%04X - EEPROM too small",
            length_in,
            address_in  
        );
        return EEPROM_ERROR_READ_TOO_LARGE;
    }

    /* Attempt to open the EEPROM file */
    FILE *fp_eeprom = fopen(EEPROM_DUMMY_FILE_PATH, "r+b");

    /* Check for success */
    if (fp_eeprom == NULL) {
        DEBUG_ERR("Could not open dummy EEPROM file");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    /* Read the file in */
    uint8_t dummy[EEPROM_SIZE_BYTES] = {0xff};
    size_t num_items = fread(dummy, sizeof(dummy), 1, fp_eeprom);

    /* Check for success */
    if (num_items != 1) {
        DEBUG_ERR("Could not read EEPROM file");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    /* Copy the data to the output buffer */
    memcpy((void *)p_data_out, (void *)&dummy[address_in], length_in);

    /* Close the file */
    if (fclose(fp_eeprom) != 0) {
        DEBUG_ERR("Could not close EEPROM dummy file");
        return EEPROM_ERROR_INIT_RECOVERY_FAILED;
    }

    return ERROR_NONE;
}