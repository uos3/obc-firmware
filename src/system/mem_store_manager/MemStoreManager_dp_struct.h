/**
 * @file MemStoreManager_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief MemStoreManager datapool structure
 * @version 0.1
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_MEMSTOREMANAGER_DP_STRUCT_H
#define H_MEMSTOREMANAGER_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_errors.h"
#include "system/mem_store_manager/MemStoreManager_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

typedef struct _MemStoreManager_Dp {
    /**
     * @brief Error code for the MemStoreManager
     */
    ErrorCode ERROR_CODE;

    /**
     * @brief Error code from the EEPROM driver.
     */
    ErrorCode EEPROM_ERROR_CODE;
} MemStoreManager_Dp;

#endif /* H_MEMSTOREMANAGER_DP_STRUCT_H */