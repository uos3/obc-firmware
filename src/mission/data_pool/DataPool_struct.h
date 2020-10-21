/**
 * @ingroup data_pool
 * 
 * @file DataPool_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Defines the members of the DataPool structure.
 * 
 * TODO
 * 
 * @version 0.1
 * @date 2020-10-21
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_DATAPOOL_STRUCT_H
#define H_DATAPOOL_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------
 * DATAPOOL STRUCT
 * ------------------------------------------------------------------------- */

/**
 * @brief The DataPool structure definition.
 */
typedef struct _DataPool {

    /**
     * @brief Flag set to true if the DataPool has been initialised, false if
     * otherwise. 
     */
    bool INITIALISED;
} DataPool;


#endif /* H_DATAPOOL_STRUCT_H */