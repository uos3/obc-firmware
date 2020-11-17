/**
 * @ingroup imu
 * @file Imu_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief IMU DataPool structure.
 * @version 0.1
 * @date 2020-11-17
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_IMU_DP_STRUCT_H
#define H_IMU_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdbool.h>
#include <stdlib.h>

/* Internal includes */
#include "components/imu/Imu_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

typedef struct _Imu_Dp {

    /**
     * @brief Flag indicating whether or not the Imu has been initialised.
     * 
     * @dp 1
     */
    bool INITIALISED;

    /**
     * @brief Stores errors that occur during operation.
     * 
     * @dp 2
     */
    Imu_ErrorCode ERROR;

    /**
     * @brief Data from the IMU's gyroscope.
     * 
     * This value is only guarenteed to be valid when EVT_IMU_GYRO_READ_SUCCESS
     * is raised.
     * 
     * @dp 3
     */
    Imu_GyroData GYROSCOPE_DATA;

    /**
     * @brief Data from the IMU's magnetometer.
     * 
     * This value is only guarenteed to be valid when 
     * EVT_IMU_MAGNE_READ_SUCCESS is raised.
     * 
     * @dp 4
     */
    Imu_MagneData MAGNETOMETER_DATA;

    /**
     * @brief Temperature reading from the IMU..
     * 
     * This value is only guarenteed to be valid when EVT_IMU_TEMP_READ_SUCCESS
     * is raised.
     * 
     * @dp 5
     */
    int16_t TEMPERATURE_DATA;

} Imu_Dp;

#endif /* H_IMU_DP_STRUCT_H */