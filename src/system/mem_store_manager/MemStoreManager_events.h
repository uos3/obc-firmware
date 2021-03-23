/**
 * @file MemStoreManager_events.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Events associated with the MemStoreManager module.
 * @version 0.1
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_MEMSTOREMANAGER_EVENTS_H
#define H_MEMSTOREMANAGER_EVENTS_H 

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_module_ids.h"
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Event indicating that a config file update succeeded.
 */
#define EVT_MEMSTOREMANAGER_CFG_UPDATE_SUCCESS ((Event)(MOD_ID_MEMSTOREMANAGER | 1))

/**
 * @brief Event indicating that a config file update failed.
 * 
 * Check DP.MEMSTOREMANAGER.ERROR_CODE for an error code.
 */
#define EVT_MEMSTOREMANAGER_CFG_UPDATE_FAILURE ((Event)(MOD_ID_MEMSTOREMANAGER | 2))

/**
 * @brief Event indicating that a corrupted config file was found while loading
 * from the EEPROM.
 * 
 * Check DP.MEMSTOREMANAGER.CFG_FILE_x_OK flags to determine which file was
 * corrupted.
 */
#define EVT_MEMSTOREMANAGER_CORRUPTED_CFG_FILE_FOUND ((Event)(MOD_ID_MEMSTOREMANAGER | 3))

/**
 * @brief Event indicating that the config files in the EEPROM couldn't be
 * loaded, so the backup config file has been loaded instead.
 */
#define EVT_MEMSTOREMANAGER_BACKUP_CFG_LOADED ((Event)(MOD_ID_MEMSTOREMANAGER | 4))

/**
 * @brief Event indicating that a corrupted persistent data file was found
 * while loading from the EEPROM.
 */
#define EVT_MEMSTOREMANAGER_CORRUPTED_PERS_FILE_FOUND ((Event)(MOD_ID_MEMSTOREMANAGER | 5))

/**
 * @brief All persistent data files (including in-memory copy) are corrupted,
 * will restore the file from a blank persistent data file.
 * 
 * THIS INDICATES ALL PERS DATA IS BEING DISCARDED!
 */
#define EVT_MEMSTOREMANAGER_RESTORE_PERS_FROM_BLANK ((Event)(MOD_ID_MEMSTOREMANAGER | 6))

/**
 * @brief Critical failure as no persistent data files were written, indicating
 * failure of the EEPROM.
 */
#define EVT_MEMSTOREMANAGER_PERS_WRITE_FAILED ((Event)(MOD_ID_MEMSTOREMANAGER | 7))

#endif /* H_MEMSTOREMANAGER_EVENTS_H */