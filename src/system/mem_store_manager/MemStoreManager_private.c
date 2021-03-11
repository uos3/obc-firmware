/**
 * @file MemStoreManager_private.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Implementation of the MemStoreManager private functions
 * @version 0.1
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "util/crypto/Crypto_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "system/mem_store_manager/MemStoreManager_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool MemStoreManager_config_check_crc(
    MemStoreManager_ConfigFile *p_cfg_file_in
) {
    /* Calculate the CRC of the data */
    Crypto_Crc32 calculated_crc;
    Crypto_get_crc32(
        (uint8_t *)&(p_cfg_file_in->data),
        sizeof(MemStoreManager_ConfigData),
        &calculated_crc
    );

    /* Perform a check to see if the CRCs match */
    return p_cfg_file_in->crc == calculated_crc;
}

bool MemStoreManager_config_load(void) {
    /* Load first config file */
    MemStoreManager_ConfigFile cfg_file_1;
    Eeprom_read(
        EEPROM_ADDR_CFG_FILE_1,
        (uint32_t *)&cfg_file_1,
        sizeof(MemStoreManager_ConfigFile)
    );

    /* Check the CRC of the first config file */
    DP.MEMSTOREMANAGER.CFG_FILE_1_OK = MemStoreManager_config_check_crc(
        &cfg_file_1
    );

    /* Load second config file */
    MemStoreManager_ConfigFile cfg_file_2;
    Eeprom_read(
        EEPROM_ADDR_CFG_FILE_2,
        (uint32_t *)&cfg_file_2,
        sizeof(MemStoreManager_ConfigFile)
    );

    /* Check the CRC of the second config file */
    DP.MEMSTOREMANAGER.CFG_FILE_2_OK = MemStoreManager_config_check_crc(
        &cfg_file_2
    );

    /* Load first config file */
    MemStoreManager_ConfigFile cfg_file_3;
    Eeprom_read(
        EEPROM_ADDR_CFG_FILE_3,
        (uint32_t *)&cfg_file_3,
        sizeof(MemStoreManager_ConfigFile)
    );

    /* Check the CRC of the first config file */
    DP.MEMSTOREMANAGER.CFG_FILE_3_OK = MemStoreManager_config_check_crc(
        &cfg_file_3
    );

    /* If any config file is not OK raise error event */
    if (!DP.MEMSTOREMANAGER.CFG_FILE_1_OK
        ||
        !DP.MEMSTOREMANAGER.CFG_FILE_2_OK
        ||
        !DP.MEMSTOREMANAGER.CFG_FILE_3_OK
    ) {
        if (!EventManager_raise_event(
            EVT_MEMSTOREMANAGER_CORRUPTED_CFG_FILE_FOUND
        )) {
            DP.MEMSTOREMANAGER.ERROR_CODE 
                = MEMSTOREMANAGER_ERROR_EVENTMANAGER_ERROR;
            return false;
        }
        DEBUG_WRN("Corrupted configuration file found!");
    }

    /* Load first non-corrupted config file */
    if (DP.MEMSTOREMANAGER.CFG_FILE_1_OK) {
        CFG = cfg_file_1.data;
    }
    else if (DP.MEMSTOREMANAGER.CFG_FILE_2_OK) {
        CFG = cfg_file_2.data;
    }
    else if (DP.MEMSTOREMANAGER.CFG_FILE_3_OK) {
        CFG = cfg_file_3.data;
    }
    /* If all corrupted raise error */
    else {
        DP.MEMSTOREMANAGER.ERROR_CODE 
            = MEMSTOREMANAGER_ERROR_ALL_CFG_FILES_CORRUPTED;

        DEBUG_WRN("Loading backup config file from software image");
        
        /* Load the backup config file */
        CFG = _binary_backup_cfg_file_start.data;
    }

    return true;
}