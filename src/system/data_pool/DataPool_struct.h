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
#include "system/mem_store_manager/MemStoreManager_dp_struct.h"
#include "system/opmode_manager/OpModeManager_dp_struct.h"
#include "components/imu/Imu_dp_struct.h"
#include "components/eps/Eps_dp_struct.h"
#include "applications/power/Power_dp_struct.h"

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
     * @brief Flag set if the Rtc driver has been initialised.
     * 
     * Note: This is located in the DataPool struct itself because Board is a
     * small module with only one global flag.
     * 
     * @dp 3
     */
    bool RTC_INITIALISED;

    /**
     * @brief DataPool parameters for the event manager.
     * 
     * @dp_module EventManager
     */
    EventManager_Dp EVENTMANAGER;

    /**
     * @brief DataPool parameters for the Imu component.
     * 
     * @dp_module Imu
     */
    Imu_Dp IMU;

    /**
     * @brief DataPool parameters for the MemStoreManager component.
     * 
     * @dp_module MemStoreManager
     */
    MemStoreManager_Dp MEMSTOREMANAGER;

    /**
     * @brief DataPool parameters for the Eps component.
     * 
     * @dp_module Eps
     */
    Eps_Dp EPS;

    /**
     * @brief DataPool parameters for the Power app.
     * 
     * @dp_module Power
     */
    Power_Dp POWER;

    /**
     * @brief DataPool parameters for the OpModeManager module.
     * 
     * @dp_module OpModeManager
     */
    OpModeManager_Dp OPMODEMANAGER;

} DataPool;


#endif /* H_DATAPOOL_STRUCT_H */