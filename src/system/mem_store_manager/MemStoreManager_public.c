/**
 * @file MemStoreManager_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides the MemStoreManager module. See corresponding .h file for
 * more information.
 * 
 * Task ref: [UT_2.9.3]
 * 
 * @version 0.1
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#ifdef DEBUG_MODE
#include <string.h>
#include <stdio.h>
#endif

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "system/mem_store_manager/MemStoreManager_private.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

MemStoreManager_ConfigData CFG;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool MemStoreManager_init(void) {
    /* If the use software image config flag is raised this indicates that the
     * EEPROM is unusable, so we shouldn't load from it. */
    if (DP.MEMSTOREMANAGER.USE_BACKUP_CFG) {
        /* Copy the backup to the global config file.
         * 
         * Note: there's no check of the CRC here, because if it's corrupted
         * that will be detected as a part of the software image integrity
         * check, and therefore we won't be running an image if it has a
         * corrupted backup config.
         */
        CFG = _binary_backup_cfg_file_start.data;
    }
    else {
        if (!MemStoreManager_config_load()) {
            return false;
        }
    }

    DP.MEMSTOREMANAGER.INITIALISED = true;

    return true;
}

bool MemStoreManager_config_update(MemStoreManager_ConfigFile *p_cfg_file_in) {
    ErrorCode eeprom_error;

    /* Check that the CRC of the new file is valid */
    if (!MemStoreManager_config_check_crc(p_cfg_file_in)) {
        DEBUG_ERR("Cannot update CFG as new CFG file CRC is invalid");
        DP.MEMSTOREMANAGER.ERROR_CODE 
            = MEMSTOREMANAGER_ERROR_NEW_CFG_CRC_INVALID;
        return false;
    }

    /* Write the first config file copy */
    eeprom_error = Eeprom_write(
        EEPROM_ADDR_CFG_FILE_1,
        (uint32_t *)p_cfg_file_in,
        sizeof(MemStoreManager_ConfigFile)
    );
    if (eeprom_error != ERROR_NONE) {
        DEBUG_ERR(
            "Error overwriting CFG file 1 to EEPROM: 0x%04X",
            eeprom_error
        );
        DP.MEMSTOREMANAGER.ERROR_CODE = MEMSTOREMANAGER_ERROR_EEPROM_ERROR;
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = eeprom_error;

        /* TODO: try to write old config back to EEPROM? */

        return false;
    }

    /* Write the second config file copy */
    eeprom_error = Eeprom_write(
        EEPROM_ADDR_CFG_FILE_2,
        (uint32_t *)p_cfg_file_in,
        sizeof(MemStoreManager_ConfigFile)
    );
    if (eeprom_error != ERROR_NONE) {
        DEBUG_ERR(
            "Error overwriting CFG file 2 to EEPROM: 0x%04X",
            eeprom_error
        );
        DP.MEMSTOREMANAGER.ERROR_CODE = MEMSTOREMANAGER_ERROR_EEPROM_ERROR;
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = eeprom_error;

        /* TODO: try to write old config back to EEPROM? */

        return false;
    }

    /* Write the third config file copy */
    eeprom_error = Eeprom_write(
        EEPROM_ADDR_CFG_FILE_3,
        (uint32_t *)p_cfg_file_in,
        sizeof(MemStoreManager_ConfigFile)
    );
    if (eeprom_error != ERROR_NONE) {
        DEBUG_ERR(
            "Error overwriting CFG file 3 to EEPROM: 0x%04X",
            eeprom_error
        );
        DP.MEMSTOREMANAGER.ERROR_CODE = MEMSTOREMANAGER_ERROR_EEPROM_ERROR;
        DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE = eeprom_error;

        /* TODO: try to write old config back to EEPROM? */

        return false;
    }

    return true;
}

#ifdef DEBUG_MODE
void MemStoreManager_debug_print_cfg(void) {
    char mode_app_list[256] = {0};
    char buff[16] = {0};
    DEBUG_INF("Loaded Configuration File:");
    DEBUG_INF("    VERSION: %d", CFG.VERSION);
    DEBUG_INF("    POWER_TASK_TIMER_DURATION_S: %u", CFG.POWER_TASK_TIMER_DURATION_S);
    DEBUG_INF("    POWER_OP_MODE_OCP_STATE_CONFIG: %u", (uint32_t)CFG.POWER_OP_MODE_OCP_STATE_CONFIG);
    DEBUG_INF("    OPMODE_APPID_TABLE: [");
    for (int mode = 0; mode < OPMODEMANAGER_NUM_OPMODES; mode++) {
        mode_app_list[0] = '\0';
        for (int i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; i++) {
            buff[0] = '\0';
            sprintf(buff, "0x%02X ", CFG.OPMODE_APPID_TABLE[mode][i]);
            strcat(mode_app_list, buff);
        }
        DEBUG_INF("        %d: %s", mode, mode_app_list);
    }
    DEBUG_INF("    ]");
}
#endif