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
 * GLOBALS
 * ------------------------------------------------------------------------- */

MemStoreManager_PersistentFile PERS;

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

bool MemStoreManager_check_pers_crc(
    MemStoreManager_PersistentFile *p_pers_file_in
) {
    /* Calculate the CRC of the data */
    Crypto_Crc32 calculated_crc;
    Crypto_get_crc32(
        (uint8_t *)&(p_pers_file_in->data),
        sizeof(MemStoreManager_PersistentData),
        &calculated_crc
    );

    /* Perform a check to see if the CRCs match */
    return p_pers_file_in->crc == calculated_crc;
}

bool MemStoreManager_load_config(void) {
    ErrorCode error;
    bool load_error = false;

    /* Load first config file */
    MemStoreManager_ConfigFile cfg_file_1;
    error = Eeprom_read(
        EEPROM_ADDR_CFG_FILE_1,
        (uint32_t *)&cfg_file_1,
        sizeof(MemStoreManager_ConfigFile)
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("EEPROM load error on file 1: 0x04X", error);
        load_error = true;
        DP.MEMSTOREMANAGER.CFG_FILE_1_OK = false;
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
    }
    else {
        /* Check the CRC of the first config file */
        DP.MEMSTOREMANAGER.CFG_FILE_1_OK = MemStoreManager_config_check_crc(
            &cfg_file_1
        );
    }


    /* Load second config file */
    MemStoreManager_ConfigFile cfg_file_2;
    error = Eeprom_read(
        EEPROM_ADDR_CFG_FILE_2,
        (uint32_t *)&cfg_file_2,
        sizeof(MemStoreManager_ConfigFile)
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("EEPROM load error on file 2: 0x04X", error);
        load_error = true;
        DP.MEMSTOREMANAGER.CFG_FILE_2_OK = false;
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
    }
    else {
        /* Check the CRC of the second config file */
        DP.MEMSTOREMANAGER.CFG_FILE_2_OK = MemStoreManager_config_check_crc(
            &cfg_file_2
        );
    }


    /* Load first config file */
    MemStoreManager_ConfigFile cfg_file_3;
    error = Eeprom_read(
        EEPROM_ADDR_CFG_FILE_3,
        (uint32_t *)&cfg_file_3,
        sizeof(MemStoreManager_ConfigFile)
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("EEPROM load error on file 3: 0x04X", error);
        load_error = true;
        DP.MEMSTOREMANAGER.CFG_FILE_2_OK = false;
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
    }
    else {
        /* Check the CRC of the first config file */
        DP.MEMSTOREMANAGER.CFG_FILE_3_OK = MemStoreManager_config_check_crc(
            &cfg_file_3
        );
    }

    /* If any config file is not OK raise error event */
    if (!DP.MEMSTOREMANAGER.CFG_FILE_1_OK
        ||
        !DP.MEMSTOREMANAGER.CFG_FILE_2_OK
        ||
        !DP.MEMSTOREMANAGER.CFG_FILE_3_OK
    ) {
        DEBUG_WRN(
            "Corrupted configuration file(s) found! Configs OK: (%d, %d, %d)",
            DP.MEMSTOREMANAGER.CFG_FILE_1_OK,
            DP.MEMSTOREMANAGER.CFG_FILE_2_OK,
            DP.MEMSTOREMANAGER.CFG_FILE_3_OK
        );
        if (!EventManager_raise_event(
            EVT_MEMSTOREMANAGER_CORRUPTED_CFG_FILE_FOUND
        )) {
            DP.MEMSTOREMANAGER.ERROR_CODE 
                = MEMSTOREMANAGER_ERROR_EVENTMANAGER_ERROR;
            /* If we can't raise the error event we will continue on to try to
             * make sure the config itself will be correct */
        }
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
        /* If this was caused by a load error use the EEPROM load error rather
         * than the generic "all files corrupted" error. */
        if (load_error) {
            DP.MEMSTOREMANAGER.ERROR_CODE 
                = MEMSTOREMANAGER_ERROR_CFG_EEPROM_LOAD_FAILED;
        }
        else {
            DP.MEMSTOREMANAGER.ERROR_CODE 
                = MEMSTOREMANAGER_ERROR_ALL_CFG_FILES_CORRUPTED;
        }

        DEBUG_WRN("Loading backup config file from software image");
        
        /* Load the backup config file */
        CFG = _binary_backup_cfg_file_start.data;

        /* Raise the backup config loaded event */
        if (!EventManager_raise_event(
            EVT_MEMSTOREMANAGER_BACKUP_CFG_LOADED
        )) {
            DP.MEMSTOREMANAGER.ERROR_CODE 
                = MEMSTOREMANAGER_ERROR_EVENTMANAGER_ERROR;
            /* As this event is only used in monitoring there's no reason to
             * exit with error if we can't raise it. */
        }
    }

    return true;
}

bool MemStoreManager_load_pers_data(void) {
    ErrorCode error;
    bool load_error = false;
    MemStoreManager_PersistentFile pers_file_1;
    MemStoreManager_PersistentFile pers_file_2;
    MemStoreManager_PersistentFile pers_file_3;

    /* Load first pers data block */
    error = Eeprom_read(
        EEPROM_ADDR_PERS_DATA_1,
        (uint32_t *)&pers_file_1,
        sizeof(MemStoreManager_PersistentFile)
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("EEPROM load error on pers file 1: 0x%04X", error);
        load_error = true;
        DP.MEMSTOREMANAGER.PERS_FILE_1_OK = false;
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
    }
    else {
        DP.MEMSTOREMANAGER.PERS_FILE_1_OK 
            = MemStoreManager_check_pers_crc(&pers_file_1);
    }

    /* Load second pers data block */
    error = Eeprom_read(
        EEPROM_ADDR_PERS_DATA_2,
        (uint32_t *)&pers_file_2,
        sizeof(MemStoreManager_PersistentFile)
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("EEPROM load error on pers file 2: 0x%04X", error);
        load_error = true;
        DP.MEMSTOREMANAGER.PERS_FILE_2_OK = false;
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
    }
    else {
        DP.MEMSTOREMANAGER.PERS_FILE_2_OK 
            = MemStoreManager_check_pers_crc(&pers_file_2);
    }

    /* Load third pers data block */
    error = Eeprom_read(
        EEPROM_ADDR_PERS_DATA_3,
        (uint32_t *)&pers_file_3,
        sizeof(MemStoreManager_PersistentFile)
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("EEPROM load error on pers file 3: 0x%04X", error);
        load_error = true;
        DP.MEMSTOREMANAGER.PERS_FILE_3_OK = false;
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
    }
    else {
        DP.MEMSTOREMANAGER.PERS_FILE_3_OK 
            = MemStoreManager_check_pers_crc(&pers_file_3);
    }

    /* Load first non-corrupted persistent file */
    if (DP.MEMSTOREMANAGER.PERS_FILE_1_OK) {
        PERS = pers_file_1;
    }
    else if (DP.MEMSTOREMANAGER.PERS_FILE_2_OK) {
        PERS = pers_file_2;
    }
    else if (DP.MEMSTOREMANAGER.PERS_FILE_3_OK) {
        PERS = pers_file_3;
    }
    /* If all corrupted we have to try and load something else. We can try to
     * see if what's already in memory would be ok, but if that's corrupted too
     * we have to discard it all and start with a blank file. 
     * 
     * This would have the impact of having to go through the first bootup
     * again with the antenna deployment. */
    else {
        /* If this was caused by a load error use the EEPROM load error rather
         * than the generic "all files corrupted" error. */
        if (load_error) {
            DP.MEMSTOREMANAGER.ERROR_CODE 
                = MEMSTOREMANAGER_ERROR_PERS_EEPROM_LOAD_FAILED;
        }
        else {
            DP.MEMSTOREMANAGER.ERROR_CODE 
                = MEMSTOREMANAGER_ERROR_ALL_PERS_FILES_CORRUPTED;
        }

        DEBUG_ERR(
            "Couldn't load any valid persistent files, attempting to restore"
        );
        
        /* If in memory file isn't valid overwrite it with emtpy data */
        if (!MemStoreManager_check_pers_crc(&PERS)) {
            DEBUG_WRN(
                "In-memory copy of persistent file is invalid, will overwrite with blank file"
            );
            memset(
                &PERS.data,
                0,
                sizeof(MemStoreManager_PersistentData)
            );

            /* Also recalcualte the CRC so the blank file is valid */
            Crypto_get_crc32(
                (uint8_t *)&PERS.data,
                sizeof(MemStoreManager_PersistentData),
                &PERS.crc                
            );

            /* Raise the restore from blank event */
            if (!EventManager_raise_event(
                EVT_MEMSTOREMANAGER_RESTORE_PERS_FROM_BLANK
            )) {
                DP.MEMSTOREMANAGER.ERROR_CODE 
                    = MEMSTOREMANAGER_ERROR_EVENTMANAGER_ERROR;
                /* As this event is only used in monitoring there's no reason 
                 * to exit with error if we can't raise it. */
            }
        }
    }

    /* Overwrite each corrupted file with the now correct in-memory file */
    if (!DP.MEMSTOREMANAGER.PERS_FILE_1_OK) {
        error = Eeprom_write(
            EEPROM_ADDR_PERS_DATA_1,
            (uint32_t *)&PERS,
            sizeof(MemStoreManager_PersistentFile)
        );
        if (error != ERROR_NONE) {
            DEBUG_ERR("Couldn't overwrite PERS file 1: 0x%04X", error);
            DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
        }
        else {
            DEBUG_TRC("PERS file 1 overwritten");
            DP.MEMSTOREMANAGER.PERS_FILE_1_OK = true;
        }
    }
    if (!DP.MEMSTOREMANAGER.PERS_FILE_2_OK) {
        error = Eeprom_write(
            EEPROM_ADDR_PERS_DATA_2,
            (uint32_t *)&PERS,
            sizeof(MemStoreManager_PersistentFile)
        );
        if (error != ERROR_NONE) {
            DEBUG_ERR("Couldn't overwrite PERS file 2: 0x%04X", error);
            DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
        }
        else {
            DEBUG_TRC("PERS file 2 overwritten");
            DP.MEMSTOREMANAGER.PERS_FILE_2_OK = true;
        }
    }
    if (!DP.MEMSTOREMANAGER.PERS_FILE_3_OK) {
        error = Eeprom_write(
            EEPROM_ADDR_PERS_DATA_3,
            (uint32_t *)&PERS,
            sizeof(MemStoreManager_PersistentFile)
        );
        if (error != ERROR_NONE) {
            DEBUG_ERR("Couldn't overwrite PERS file 3: 0x%04X", error);
            DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
        }
        else {
            DEBUG_TRC("PERS file 3 overwritten");
            DP.MEMSTOREMANAGER.PERS_FILE_3_OK = true;
        }
    }

    return true;
}

bool MemStoreManager_write_pers_data(void) {
    ErrorCode error;

    /* If the current PERS CRC is invalid we error out here, don't attempt to
     * fix it as the caller is responsible for this */
    if (!MemStoreManager_check_pers_crc(&PERS)) {
        DEBUG_ERR("Attempted to write invalid PERS file to EEPROM");
        DP.MEMSTOREMANAGER.ERROR_CODE 
            = MEMSTOREMANAGER_ERROR_WRITE_INVALID_PERS;
        return false;
    }

    /* If file is valid write it out */
    error = Eeprom_write(
        EEPROM_ADDR_PERS_DATA_1,
        (uint32_t *)&PERS,
        sizeof(MemStoreManager_PersistentFile)
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("Couldn't overwrite PERS file 1: 0x%04X", error);
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
        DP.MEMSTOREMANAGER.ERROR_CODE = MEMSTOREMANAGER_ERROR_EEPROM_ERROR;
        DP.MEMSTOREMANAGER.PERS_FILE_1_OK = false;
        /* Rather than exiting here we should try to write to the other files
         * too, that way we may be able to store in some of the backups */
    }
    else {
        DP.MEMSTOREMANAGER.PERS_FILE_1_OK = true;
    }

    error = Eeprom_write(
        EEPROM_ADDR_PERS_DATA_2,
        (uint32_t *)&PERS,
        sizeof(MemStoreManager_PersistentFile)
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("Couldn't overwrite PERS file 2: 0x%04X", error);
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
        DP.MEMSTOREMANAGER.ERROR_CODE = MEMSTOREMANAGER_ERROR_EEPROM_ERROR;
        DP.MEMSTOREMANAGER.PERS_FILE_2_OK = false;
        /* Rather than exiting here we should try to write to the other files
         * too, that way we may be able to store in some of the backups */
    }
    else {
        DP.MEMSTOREMANAGER.PERS_FILE_2_OK = true;
    }

    error = Eeprom_write(
        EEPROM_ADDR_PERS_DATA_3,
        (uint32_t *)&PERS,
        sizeof(MemStoreManager_PersistentFile)
    );
    if (error != ERROR_NONE) {
        DEBUG_ERR("Couldn't overwrite PERS file 3: 0x%04X", error);
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = error;
        DP.MEMSTOREMANAGER.ERROR_CODE = MEMSTOREMANAGER_ERROR_EEPROM_ERROR;
        DP.MEMSTOREMANAGER.PERS_FILE_3_OK = false;
        /* Rather than exiting here we should try to write to the other files
         * too, that way we may be able to store in some of the backups */
    }
    else {
        DP.MEMSTOREMANAGER.PERS_FILE_3_OK = true;
    }

    /* If all files are invalid the write completely failed, indicating the
     * EEPROM itself is broken, which is very bad.
     * 
     * Realistically I doubt there's any way for the system to continue to
     * function if the EEPROM is dead, so we just return and leave this to 
     * FDIR */
    if (DP.MEMSTOREMANAGER.PERS_FILE_1_OK
        && !DP.MEMSTOREMANAGER.PERS_FILE_2_OK
        && !DP.MEMSTOREMANAGER.PERS_FILE_3_OK
    ) {
        DEBUG_ERR("Couldn't write any PERS files, recovery not possible");
        /* Don't want to override the EEPROM error, instead raise the write
         * failed event, which will be picked up by FDIR */
        if (!EventManager_raise_event(
            EVT_MEMSTOREMANAGER_PERS_WRITE_FAILED
        )) {
            DEBUG_ERR("Couldn't raise EVT_MEMSTOREMANAGER_PERS_WRITE_FAILED");
        }
        return false;
    }

    return true;
}