/**
 * @file MemStoreManager_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Errors that can occur in the MemStoreManager module.
 * @version 0.1
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_MEMSTOREMANAGER_ERRORS_H
#define H_MEMSTOREMANAGER_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_module_ids.h"
#include "system/kernel/Kernel_errors.h"

/* -------------------------------------------------------------------------   
 * ERRORS
 * ------------------------------------------------------------------------- */

/**
 * @brief A config file update was attempted with a config file that has an
 * invalid CRC.
 */
#define MEMSTOREMANAGER_ERROR_NEW_CFG_CRC_INVALID ((ErrorCode)(MOD_ID_MEMSTOREMANAGER | 1))

/**
 * @brief An error occured in the EEPROM driver.
 */
#define MEMSTOREMANAGER_ERROR_EEPROM_ERROR ((ErrorCode)(MOD_ID_MEMSTOREMANAGER | 2))

/**
 * @brief Indicates an error occured in the event manager which is being
 * propagated.
 */
#define MEMSTOREMANAGER_ERROR_EVENTMANAGER_ERROR ((ErrorCode)(MOD_ID_MEMSTOREMANAGER | 3))

/**
 * @brief All stored configuration files are corrupted.
 */
#define MEMSTOREMANAGER_ERROR_ALL_CFG_FILES_CORRUPTED ((ErrorCode)(MOD_ID_MEMSTOREMANAGER | 4))

/**
 * @brief Unable to load config files as EEPROM load failed. See
 * DP.MEMESTOREMANAGER.EEPROM_ERROR_CODE for root cause.
 */
#define MEMSTOREMANAGER_ERROR_CFG_EEPROM_LOAD_FAILED ((ErrorCode)(MOD_ID_MEMSTOREMANAGER | 5))

/**
 * @brief All stored persistent data files are corrupted.
 */
#define MEMSTOREMANAGER_ERROR_ALL_PERS_FILES_CORRUPTED ((ErrorCode)(MOD_ID_MEMSTOREMANAGER | 6))

/**
 * @brief Unable to load persistent files as the EEPROM load failed. See
 * DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE for root cause.
 */
#define MEMSTOREMANAGER_ERROR_PERS_EEPROM_LOAD_FAILED ((ErrorCode)(MOD_ID_MEMSTOREMANAGER | 7))

/**
 * @brief Attempted to write an invalid persistent data file to the EEPROM.
 */
#define MEMSTOREMANAGER_ERROR_WRITE_INVALID_PERS ((ErrorCode)(MOD_ID_MEMSTOREMANAGER | 8))

#endif /* H_MEMSTOREMANAGER_ERRORS_H */