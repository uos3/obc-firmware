/**
 * @file tool_config_flash.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Flash the config file to the EEPROM.
 * 
 * This file is intended to be flashed onto the TM4C, on booting it will write
 * a predefined number of copies of the config file to the EEPROM.
 * 
 * @version 0.1
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* System includes */
#include <stdint.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "drivers/eeprom/Eeprom_private.h"
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "system/mem_store_manager/MemStoreManager_private.h"

/* -------------------------------------------------------------------------   
 * BINARY CONFIG FILE
 * ------------------------------------------------------------------------- */

/**
 * @brief Location of the configuration file.
 * 
 * The config file is passed in at link time from a raw binary file created by
 * tool_config_pack. See
 * http://downloads.ti.com/docs/esd/SPRUI03/using-linker-symbols-in-c-c-applications-slau1318080.html
 * for some info on linker symbols and how to use them.
 * 
 * The name used here is set by the linker itself and can't be changed.
 */
extern MemStoreManager_ConfigFile _binary_cfg_file_start;

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {

    /* Init system critical modules */
    if (!DataPool_init()) {
        Debug_exit(1);
    }
    if (!Board_init()) {
        Debug_exit(1);
    }
    if (!Debug_init()) {
        Debug_exit(1);
    }
    if (!EventManager_init()) {
        Debug_exit(1);
    }
    if (Eeprom_init() != ERROR_NONE) {
        Debug_exit(1);
    }

    /* Create local copy of the config file using a better name. */
    MemStoreManager_ConfigFile cfg_file = _binary_cfg_file_start;

    /* Print out the config file version number */
    DEBUG_INF(
        "tool_config_flash loaded with config file version %d", 
        cfg_file.data.VERSION
    );

    /* Check the CRCs match */
    DEBUG_INF("Checking CRC");
    if (!MemStoreManager_config_check_crc(&cfg_file)) {
        DEBUG_ERR("CRC check failed");
        Debug_exit(1);
    }
    DEBUG_INF("CRC check passed");

    /* Calculate the total size of the configuration files */
    uint32_t config_files_size_bytes = 
        MEMSTOREMANAGER_NUM_CONFIG_FILES 
            * sizeof(MemStoreManager_ConfigFile);
    
    /* Check that the correct number of config files will fit in the EEPROM */
    if (config_files_size_bytes >= EEPROM_SIZE_BYTES) {
        DEBUG_ERR(
"Cannot write config files to EEPROM. Each config is %d bytes long so %d \
configs would use %d bytes of EEPROM, but the EEPROM is only %d bytes long.",
            sizeof(MemStoreManager_ConfigFile),
            MEMSTOREMANAGER_NUM_CONFIG_FILES,
            config_files_size_bytes,
            EEPROM_SIZE_BYTES
        );
    }

    /* Write files to the EEPROM */
    if (!MemStoreManager_config_update(&cfg_file)) {
        DEBUG_ERR(
            "Configuration file update failed: 0x%04X", 
            DP.MEMSTOREMANAGER.ERROR_CODE
        );
        Debug_exit(1);
    }

    DEBUG_INF("CFG files written to EEPROM");

    /* Trigger breakpoint */
    Debug_exit(0);
}