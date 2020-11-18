/**
 * @ingroup imu
 * @file Imu_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief IMU component private header file.
 * 
 * Task ref: [UT_2.10.1]
 * 
 * @version 0.1
 * @date 2020-11-17
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_IMU_PRIVATE_H
#define H_IMU_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "components/imu/Imu_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Setup the first execution of the SET_GYROSCOPE_OFFSETS state.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_begin_set_gyro_offsets(void);

/**
 * @brief Setup the first execution of the SELF_TEST state.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_begin_self_test(void);

/**
 * @brief Setup the first execution of the WAIT_NEW_COMMAND state.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_begin_wait_new_command(void);

/**
 * @brief Setup the first execution of the READ_TEMPERATURE state.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_begin_read_temp(void);

/**
 * @brief Setup the first execution of the READ_GYROSCOPE state.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_begin_read_gyro(void);

/**
 * @brief Setup the first execution of the READ_MAGNETOMETER state.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_begin_read_magne(void);

/**
 * @brief Step the SET_GYROSCOPE_OFFSETS state machine.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_step_set_gyro_offsets(void);

/**
 * @brief Step the SELF_TEST state machine.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_step_self_test(void);

/**
 * @brief Step the READ_TEMPERATURE state machine.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_step_read_temp(void);

/**
 * @brief Step the READ_GYROSCOPE state machine.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_step_read_gyro(void);

/**
 * @brief Step the READ_MAGNETOMETER state machine.
 * 
 * This function will log an error message and set the appropriate value of
 * DP.IMU.ERROR if it returns false.
 * 
 * @return bool True on success, false on failure.
 */
bool Imu_step_read_magne(void);


#endif /* H_IMU_PRIVATE_H */