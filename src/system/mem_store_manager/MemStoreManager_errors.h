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

#endif /* H_MEMSTOREMANAGER_ERRORS_H */