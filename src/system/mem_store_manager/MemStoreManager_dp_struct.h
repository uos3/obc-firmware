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
     * 
     * @dp 1
     */
    ErrorCode ERROR_CODE;

    /**
     * @brief Error code from the EEPROM driver.
     * 
     * @dp 2
     */
    ErrorCode EEPROM_ERROR_CODE;

    /**
     * @brief Indicates if the first configuration file is OK.
     * 
     * @dp 3
     */
    bool CFG_FILE_1_OK;

    /**
     * @brief Indicates if the second configuration file is OK.
     * 
     * @dp 4
     */
    bool CFG_FILE_2_OK;

    /**
     * @brief Indicates if the third configuration file is OK.
     * 
     * @dp 5
     */
    bool CFG_FILE_3_OK;
} MemStoreManager_Dp;

#endif /* H_MEMSTOREMANAGER_DP_STRUCT_H */