/**
 * @ingroup imu
 * @file Imu_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Defines errors within the IMU module.
 * 
 * Task ref: [UT_2.10.1]
 * 
 * @version 0.1
 * @date 2020-11-30
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_IMU_ERRORS_H
#define H_IMU_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Attempted to use the IMU when it wasn't initialised.
 * 
 */
#define IMU_ERROR_NOT_INITIALISED ((ErrorCode)(MOD_ID_IMU | 1))

/**
 * @brief Attempted to issue an IMU_CMD_NONE to the IMU module.
 */
#define IMU_ERROR_NEW_NONE_COMMAND ((ErrorCode)(MOD_ID_IMU | 2))

/**
 * @brief There was an error in the EventManager that is preventing the IMU
 * component operating. 
 */
#define IMU_ERROR_EVENTMANAGER_ERROR ((ErrorCode)(MOD_ID_IMU | 3))

/**
 * @brief Invalid DP.IMU.STATE value.
 */
#define IMU_ERROR_INVALID_STATE ((ErrorCode)(MOD_ID_IMU | 4))

/**
 * @brief Invalid DP.IMU.SUBSTATE value.
 */
#define IMU_ERROR_INVALID_SUBSTATE ((ErrorCode)(MOD_ID_IMU | 5))

/**
 * @brief Unrecognised DP.IMU.COMMAND value.
 * 
 */
#define IMU_ERROR_UNRECOGNISED_CMD ((ErrorCode)(MOD_ID_IMU | 6))

/**
 * @brief Cannot issue a new command to the IMU since the IMU is not in the
 * WAIT_NEW_COMMAND state. It could either not be initialised or is still
 * finishing another command 
 */
#define IMU_ERROR_CANNOT_ISSUE_NEW_COMMAND ((ErrorCode)(MOD_ID_IMU | 7))

/**
 * @brief There was an error in the I2C driver.
 * 
 * See the corresponding value of DP.IMU.I2C_ERROR for the root cause.
 */
#define IMU_ERROR_I2C_ERROR ((ErrorCode)(MOD_ID_IMU | 8))

/**
 * @brief Unexpected value of an I2c_ActionStatus return.
 */
#define IMU_ERROR_INVALID_I2C_ACTION_STATUS ((ErrorCode)(MOD_ID_IMU | 9))

#endif /* H_IMU_ERRORS_H */