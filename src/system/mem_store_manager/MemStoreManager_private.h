/**
 * @file MemStoreManager_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief MemStoreManager module private header.
 * 
 * Task ref: [UT_2.9.3]
 * 
 * @version 0.1
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_MEMSTOREMANAGER_PRIVATE_H
#define H_MEMSTOREMANAGER_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* System includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "system/mem_store_manager/MemStoreManager_events.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Number of redundant configuration files to store in the EEPROM.
 * 
 * NOTE: This value is arbitraray.
 */
#define MEMSTOREMANAGER_NUM_CONFIG_FILES (3)

/* -------------------------------------------------------------------------   
 * GLOBALS  
 * ------------------------------------------------------------------------- */

/**
 * @brief Location of the backup software image configuration file.
 * 
 * The config file is passed in at link time from a raw binary file created by
 * tool_config_pack. See
 * http://downloads.ti.com/docs/esd/SPRUI03/using-linker-symbols-in-c-c-applications-slau1318080.html
 * for some info on linker symbols and how to use them.
 * 
 * The name used here is set by the linker itself and can't be changed.
 */
extern MemStoreManager_ConfigFile _binary_backup_cfg_file_start;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Check that the computed CRC of the configuration data matches that of
 * the stored CRC in the configuration file.
 * 
 * Note no error code is set on failure, as this is a simple pass/fail check.
 * 
 * @param p_cfg_file_in Configuration file to check.
 * @return bool True on success, false on failure.
 */
bool MemStoreManager_config_check_crc(
    MemStoreManager_ConfigFile *p_cfg_file_in
);

/**
 * @brief Load the configuration files from the EEPROM.
 * 
 * This function loads the config data by reading config files from the EEPROM.
 * Note only one non-corrupted config file is required for success. If a
 * corrupted config file is found it is marked as not-ok in the datapool and
 * the EVT_MEMSTOREMANAGER_CORRUPTED_CFG_FILE_FOUND event is fired.
 * 
 * Only if all config files are corrupted will the loading fail, in which case
 * false is returned and the datapool DP.MEMSTOREMANAGER.ERROR_CODE value is
 * set. 
 * 
 * @return bool True on success, false on failure.
 */
bool MemStoreManager_config_load(void);

#endif /* H_MEMSTOREMANAGER_PRIVATE_H */