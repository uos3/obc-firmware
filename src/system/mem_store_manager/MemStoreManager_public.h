/**
 * @file MemStoreManager_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides the MemStoreManager module
 * 
 * Task ref: [UT_2.9.3]
 * 
 * MemStoreManager is responsible for the memory and storage of the CubeSat.
 * This generally falls into three main functions:
 * 
 * 1. Validating the software images stored in the TM4C's flash
 * 2. Managing the configuration files stored in the TM4C's EEPROM
 * 3. Managing the telemetry buffer stored in the TOBC FRAM chip
 * 
 * TODO: Points 1 and 3 will be implemented at a later date.
 * 
 * ## Configuration
 * 
 * The spacecraft's configuration file is stored in the TM4C EEPROM chip, which
 * is a non-volatile 2KB storage chip. The configuration file contains all
 * parameters that may be adjusted in flight by the ground control, for
 * instance silent mode of the spacecraft and payload acquisition intervals are
 * stored in the configuration file.
 * 
 * To protect against Single Event Upsets (SEUs) multiple configuration files
 * are stored in the EEPROM. Each file contains a Cyclic Redundency Check (CRC)
 * code, which can be used to verify that the particular file is not corrupted
 * when loaded.
 * 
 * @version 0.1
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_MEMSTOREMANAGER_PUBLIC_H
#define H_MEMSTOREMANAGER_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* System includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "system/mem_store_manager/MemStoreManager_errors.h"
#include "system/mem_store_manager/MemStoreManager_events.h"
#include "applications/power/Power_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Configuration data for the spacecraft.
 * 
 * The configuration data stores any parameter that may need to be adjsted by
 * ground control.
 */
typedef struct _MemStoreManager_ConfigData {
    /**
     * @brief The version number of the configuration data.
     */
    uint8_t version;

    /**
     * @brief Duration of the Power app's primary task timer, in seconds.
     */
    uint16_t power_task_timer_duration_s;

    /**
     * @brief The bit vector which defines which OCP rails shall be enabled
     * during which OpModes (except the OBC and EPS rail, which are always on).
     */
    Power_OpModeOcpStateConfig power_op_mode_ocp_rail_config;

} MemStoreManager_ConfigData;

/**
 * @brief A configuration file.
 * 
 * The config file stores the config data plus a CRC, and represents the raw
 * data stored in the EEPROM.
 */
typedef struct _MemStoreManager_ConfigFile {
    
    /**
     * @brief The configuration data stored in this file.
     */
    MemStoreManager_ConfigData data;

    /**
     * @brief The CRC of the configuration file.
     */
    uint32_t crc;
} MemStoreManager_ConfigFile;

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

extern MemStoreManager_ConfigData CFG;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the MemStoreManager.
 * 
 * This function:
 * 1. Loads the configuration from the EEPROM.
 * 
 * On an error the DP.MEMSTOREMANAGER.ERROR_CODE value will be set.
 * 
 * @return bool True on success, false on failure.
 */
bool MemStoreManager_init(void);

/**
 * @brief Step the memory and storage manager.
 * 
 * On an error the DP.MEMSTOREMANAGER.ERROR_CODE value will be set.
 * 
 * @return bool True on success, false on failure.
 */
bool MemStoreManager_step(void);

/**
 * @brief Update the configuration files stored in the EEPROM to the given
 * config file.
 * 
 * This is a blocking function which will return only when the operation is
 * completed. If false is returned the update failed and any changes made
 * during the update have been reverted. An event is also emmitted on success
 * or failure.
 * 
 * @param p_cfg_file_in The configuration file to use.
 * @return bool True on success, false on failure.
 */
bool MemStoreManager_config_update(MemStoreManager_ConfigFile *p_cfg_file_in);

#endif /* H_MEMSTOREMANAGER_PUBLIC_H */