
/**
 * @ingroup data_pool
 * 
 * @file DataPool_generated.h
 * @author Generated by DataPool_generate.py
 * @brief Generated code to assist with DataPool usage.
 * 
 * This file was generated from DataPool_struct.h by DataPool_generate.py.
 * 
 * @version Generated from DataPool_struct.h version 0.1
 * @date 2021-02-17
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_DATAPOOL_GENERATED_H
#define H_DATAPOOL_GENERATED_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Internal includes */
#include "system/event_manager/EventManager_dp_struct.h"
#include "system/mem_store_manager/MemStoreManager_dp_struct.h"
#include "components/imu/Imu_dp_struct.h"
#include "components/eps/Eps_dp_struct.h"

/* -------------------------------------------------------------------------
 * TYPES
 * ------------------------------------------------------------------------- */

/**
 * @brief Type representing an ID to access the DataPool.
 */
typedef uint16_t DataPool_Id;

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/**
 * @brief Types of data that are contained in the DataPool.
 */
typedef enum _DataPool_DataType {
    DATAPOOL_DATATYPE_BOOL,
    DATAPOOL_DATATYPE_ERRORCODE,
    DATAPOOL_DATATYPE_UINT8_T,
    DATAPOOL_DATATYPE_SIZE_T,
    DATAPOOL_DATATYPE_IMU_STATE,
    DATAPOOL_DATATYPE_IMU_SUBSTATE,
    DATAPOOL_DATATYPE_IMU_COMMAND,
    DATAPOOL_DATATYPE_IMU_VECINT16,
    DATAPOOL_DATATYPE_IMU_VECUINT8,
    DATAPOOL_DATATYPE_INT16_T,
    DATAPOOL_DATATYPE_EPS_STATE,
    DATAPOOL_DATATYPE_EPS_COMMANDSTATUS,
    DATAPOOL_DATATYPE_EPS_HKDATA
} DataPool_DataType;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Get a pointer to the DataPool parameter associated with the given ID,
 * as well as the type of the data and the number of bytes the data occupies.
 * 
 * @param id_in The ID of the DataPool parameter.
 * @param p_data_out Output pointer to a pointer to the parameter.
 * @param p_data_type_out Output pointer to the data type of the parameter.
 * @param p_data_size_out Output pointer to the size (in bytes) of the 
 *        parameter.
 * @return bool True if successful, false if invalid id.
 */
bool DataPool_get(
    DataPool_Id id_in,
    void **pp_data_out,
    DataPool_DataType *p_data_type_out,
    size_t *p_data_size_out
);

/**
 * @brief Fill a buffer with the symbol name of the DataPool parameter with the
 * given ID.
 *
 * The caller is responsible for freeing the memory allocated for the symbol
 * string.
 *
 * @param id_in The ID of the DataPool parameter.
 * @param pp_symbol_str_out Pointer to the buffer to store the string in.
 * @return bool True if successful, false if the ID is invalid.
 */
bool DataPool_get_symbol_str(
    DataPool_Id id_in, 
    char **pp_symbol_str_out
);

#endif /* H_DATAPOOL_GENERATED_H */
