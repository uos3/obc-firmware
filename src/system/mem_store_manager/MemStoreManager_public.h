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
#include "system/kernel/Kernel_public.h"
#include "system/opmode_manager/OpModeManager_public.h"
#include "applications/power/Power_public.h"
#include "system/mem_store_manager/MemStoreManager_errors.h"
#include "system/mem_store_manager/MemStoreManager_events.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Configuration data for the spacecraft.
 * 
 * The configuration data stores any parameter that may need to be adjsted by
 * ground control.
 * 
 * NOTE: This structure should be as flat as possible (no nested structs), as
 * nesting requires word alignment which can make the config file larger than
 * it needs to be.
 * 
 * NOTE: UPPER_SNAKE_CASE is used here as this data is treated as global
 * read-only state, i.e. is accessed through CFG.VERSION.
 */
typedef struct _MemStoreManager_ConfigData {
    /**
     * @brief The version number of the configuration data.
     */
    uint8_t VERSION;

    /**
     * @brief Duration of the Power app's primary task timer, in seconds.
     */
    uint16_t POWER_TASK_TIMER_DURATION_S;

    /**
     * @brief The bit vector which defines which OCP rails shall be enabled
     * during which OpModes (except the OBC and EPS rail, which are always on).
     */
    Power_OpModeOcpStateConfig POWER_OPMODE_OCP_STATE_CONFIG;

    /**
     * @brief The ADC value of Eps_HkData.vbatt_vsense used as the threshold
     * for low power mode.
     * 
     * This is determined using the following logic:
     * if (DP.EPS.HK_DATA.vbatt_vsense 
     *    <=
     *    CFG.POWER_VBATT_VSENSE_LOW_POWER_THRESHOLD 
     * ) {
     *    TRANSITION TO LOW POWER MODE
     * }
     */
    uint16_t POWER_VBATT_VSENSE_LOW_POWER_THRESHOLD;

    /**
     * @brief An Eps_OcpByte which determines whether or not the EPS will reset
     * an OCP rail after its OCP has been tripped.
     */
    uint8_t EPS_RESET_RAIL_AFTER_OCP;

    /**
     * @brief The length of the EPS's watchdog timer for the TOBC, in EPS MCU
     * clock cycles.
     * 
     * To get a value in seconds divide by 4096.
     */
    uint16_t EPS_TOBC_TIMER_LENGTH;

    /**
     * @brief Table defining which app is active in which OpMode.
     * 
     * Note on efficiency:
     * 
     * With this config we specify the entire table of OpMode/AppIds,
     * including those which don't have any apps. This is pretty inefficient as
     * we need to include a bunch of 0 bytes. However, the alternative requires
     * us storing an array of:
     *   - mode number
     *   - number of apps in the mode
     *   - the app ids themselves
     * 
     * Which obviously must be of variable length, therefore we must specify 
     * the length of the table. In total, for 8 modes and 2 a maximum of 2 app
     * per mode, we get 16 bytes for the full table, or 11 bytes for the more 
     * efficient method (with the current OPMODE). In addition having a 
     * dynamically sized item in the config requires heap allocating some of 
     * the config. We would like to avoid this if at all possible. If the 
     * maximum number of modes increases we may have to switch to the other 
     * method, as that would require 24 bytes, more than twice the dynamic 
     * method.
     */
    Kernel_AppId OPMODE_APPID_TABLE[OPMODEMANAGER_NUM_OPMODES][OPMODEMANAGER_MAX_NUM_APPS_IN_MODE];

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

/**
 * @brief The persistent data stored in the EEPROM that is used to monitor
 * overall mission progression.
 */
typedef struct _MemStoreManager_PersistentData {

    /**
     * @brief Number of times the OBC has reset
     */
    uint16_t num_obc_resets;

    /**
     * @brief The number of attempts made at deploying the antenna
     */
    uint8_t num_antenna_deploy_attempts;

    /**
     * @brief Whether or not the antenna has successfully deployed.
     */
    bool antenna_deployed;

    /**
     * @brief The OpMode before the last reset
     */
    OpModeManager_OpMode last_opmode;

} MemStoreManager_PersistentData;

/**
 * @brief The persistent file stored in the EEPROM, containing the CRC of the
 * data itself.
 */
typedef struct _MemStoreManager_PersistentFile {

    /**
     * @brief The data stored in this file
     */
    MemStoreManager_PersistentData data;

    /**
     * @brief CRC of the persistant data, used to scrub for errors in the
     * persistant data.
     */
    Crypto_Crc32 crc;

} MemStoreManager_PersistentFile;

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief Global configuration file instance.
 */
extern MemStoreManager_ConfigData CFG;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the MemStoreManager.
 * 
 * This function:
 *  1. Checks the validity of all config files
 *  2. Loads the first non-corrupted file
 *  3. If all files are corrupted, it will load the default files.
 * 
 * If the EEPROM driver failed to initialise the backups can be loaded by
 * setting the DP.MEMSTOREMANAGER.USE_BACKUP_CFG flag before calling this
 * function. 
 * 
 * On an error the DP.MEMSTOREMANAGER.ERROR_CODE value will be set.
 * 
 * @return bool True on success, false on failure.
 */
bool MemStoreManager_init(void);

/**
 * @brief Step the memory and storage manager.
 * 
 * This function is responsible for:
 *  - Scrubbing any corruption to the in-memory config file instance
 *  - Writing any changes to the persistant data to the EEPROM
 *  - TODO: Maintaing buffer
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

/**
 * @brief Get a modifiable copy of the persistent data.
 * 
 * The persistent data can change at any point in execution, so the user must
 * call this function immediately before processing or modifying the data
 * contained within it.
 * 
 * This function will check for corruption of the data and make corrections if
 * needed. 
 * 
 * @return The current persistent data.
 */
MemStoreManager_PersistentData MemStoreManager_get_pers_data(void);

/**
 * @brief Set the persistent data.
 * 
 * If the user wishes to modify the persistent data they must first use
 * MemStoreManager_get_pers_data to get an up-to-date version of the data, then
 * they must modify that copy, and finally save it using this function.
 * 
 * This function will automatically set the new CRC of the data.
 * 
 * @param pers_data_in The data to set
 */
void MemStoreManager_set_pers_data(
    MemStoreManager_PersistentData pers_data_in
);

#ifdef DEBUG_MODE
/**
 * @brief Debug function used to print the configuration file to the primary
 * debug UART.
 */
void MemStoreManager_debug_print_cfg(void);

/**
 * @brief Debug function to print the current persisitent data to the primary
 * debug UART.
 */
void MemStoreManager_debug_print_pers(void);
#endif

#endif /* H_MEMSTOREMANAGER_PUBLIC_H */