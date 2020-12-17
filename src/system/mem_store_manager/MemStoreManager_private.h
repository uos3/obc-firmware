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

#endif /* H_MEMSTOREMANAGER_PRIVATE_H */