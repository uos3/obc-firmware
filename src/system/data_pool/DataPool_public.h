/**
 * @ingroup system
 * 
 * @file DataPool_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides DataPool management and access functionality.
 * 
 * The DataPool provides a single store of observable information in the OBSW.
 * 
 * TODO
 * 
 * @version 0.1
 * @date 2020-10-21
 * 
 * @copyright Copyright (c) UoS3 2020
 * 
 * @defgroup data_pool Data Pool
 * @{
 */

#ifndef H_DATAPOOL_PUBLIC_H
#define H_DATAPOOL_PUBLIC_H

/* -------------------------------------------------------------------------
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdbool.h>

/* Internal includes */
#include "system/data_pool/DataPool_struct.h"
#include "system/data_pool/DataPool_generated.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief The single global DataPool instance.
 */
extern DataPool DP;

/* -------------------------------------------------------------------------
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the DataPool.
 * 
 * @return bool Return code, true indicates success.
 */
bool DataPool_init(void);

#endif /* H_DATAPOOL_PUBLIC_H */

/** @} */ /* End of data_pool */