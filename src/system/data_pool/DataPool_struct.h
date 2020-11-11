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

/* Internal includes */
#include "system/event_manager/EventManager_dp_struct.h"

/* -------------------------------------------------------------------------
 * DATAPOOL STRUCT
 * ------------------------------------------------------------------------- */

/**
 * @brief The DataPool structure definition.
 * 
 * ---- BLOCK ASSIGNMENTS ----
 * DataPool: 0
 * EventManager: 1
 */
typedef struct _DataPool {

    /**
     * @brief Flag set to true if the DataPool has been initialised, false if
     * otherwise. 
     * 
     * @dp 1
     */
    bool INITIALISED;

    /**
     * @brief Flag set to true if the Board driver has been initialised, false
     * otherwise. 
     * 
     * Note: This is located in the DataPool struct itself because Board is a
     * small module with only one global flag.
     * 
     * @dp 2
     */
    bool BOARD_INITIALISED;

    /**
     * @brief DataPool parameters for the event manager.
     * 
     * @dp_module EventManager
     */
    EventManager_Dp EVENTMANAGER;
} DataPool;


#endif /* H_DATAPOOL_STRUCT_H */