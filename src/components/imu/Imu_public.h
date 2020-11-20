/**
 * @ingroup component
 * @file Imu_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief IMU component module.
 * 
 * Task ref: [UT_2.10.1]
 * 
 * This module provides an interface to manage and use the onboard IMU (an 
 * MPU-9250A).
 * 
 * Three main reference documents are used in this module:
 *  - [PRODSPEC] - MPU-9250 Product Specification v1.1
 *  - [REGMAP] - MPU-9250 Register Map and Descriptions v1.6
 *  - [SELFTEST] - AN-MPU-9250A-03 MPU-9250 Accel Gyro and Compass Self-Test 
 *      Implementation
 * 
 * Calibration tooling is provided so that the IMU can be easily calibrated
 * using the `tools/tool_imu_calibrate` executable. Including the calibration
 * code is enabled by passing the `imu_calib` flag to the build script.
 * 
 * Note: Magno was previously used as a shortening of magnetometer, however
 * this has been replaced by magne.
 * 
 * @version 0.1
 * @date 2020-11-17
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup imu IMU
 * @{
 */

#ifndef H_IMU_PUBLIC_H
#define H_IMU_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdbool.h>

/* Internal includes */
#ifdef F_IMU_CALIB
#include "components/imu/Imu_calib.h"
#endif

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

typedef enum _Imu_Command {
    /**
     * @brief No command. This is the default state upon initialisation of the
     * IMU and DataPool.
     */
    IMU_CMD_NONE = 0,

    /**
     * @brief Read data from the IMU's gyroscope.
     * 
     * The caller should await the EVT_IMU_READ_GYRO_SUCCESS event, at which
     * point the data can be read from DP.IMU.GYROSCOPE_DATA. If an error
     * occured EVT_IMU_READ_GYRO_FAILURE will be emmitted, after which
     * DP.IMU.ERROR will be set with an Imu_ErrorCode.
     */
    IMU_CMD_READ_GYROSCOPE,

    /**
     * @brief Read data from the IMU's magnetometer.
     * 
     * The caller should await the EVT_IMU_READ_MAGNE_SUCCESS event, at which
     * point the data can be read from DP.IMU.MAGNETOMETER_DATA. If an error
     * occured EVT_IMU_READ_MAGNE_FAILURE will be emmitted, after which
     * DP.IMU.ERROR will be set with an Imu_ErrorCode.
     */
    IMU_CMD_READ_MAGNETOMETER,

    /* TODO: Accelerometer command too? */

    /**
     * @brief Read the temperature from the IMU.
     * 
     * The caller should await the EVT_IMU_READ_TEMP_SUCCESS event, at which
     * point the data can be read from DP.IMU.TEMPERATURE_DATA. If an error
     * occured EVT_IMU_READ_TEMP_FAILURE will be emmitted, after which
     * DP.IMU.ERROR will be set with an Imu_ErrorCode. 
     */
    IMU_CMD_READ_TEMPERATURE
} Imu_Command;

typedef enum _Imu_ErrorCode {

    /**
     * @brief Indicates that no error has occured.
     */
    IMU_ERROR_NONE = 0,

    /**
     * @brief Attempted to use the IMU when it wasn't initialised.
     * 
     */
    IMU_ERROR_NOT_INITIALISED,

    /**
     * @brief Attempted to issue an IMU_CMD_NONE to the IMU module.
     */
    IMU_ERROR_NEW_NONE_COMMAND,

    /**
     * @brief There was an error in the EventManager that is preventing the IMU
     * component operating. 
     */
    IMU_ERROR_EVENTMANAGER_ERROR,

    /**
     * @brief Invalid DP.IMU.STATE value.
     */
    IMU_ERROR_INVALID_STATE,

    /**
     * @brief Invalid DP.IMU.SUBSTATE value.
     */
    IMU_ERROR_INVALID_SUBSTATE,

    /**
     * @brief Unrecognised DP.IMU.COMMAND value.
     * 
     */
    IMU_ERROR_UNRECOGNISED_CMD,

    /**
     * @brief Cannot issue a new command to the IMU since the IMU is not in the
     * WAIT_NEW_COMMAND state. It could either not be initialised or is still
     * finishing another command 
     */
    IMU_ERROR_CANNOT_ISSUE_NEW_COMMAND,

    /**
     * @brief There was an error in the I2C driver.
     * 
     * See the corresponding value of DP.IMU.I2C_ERROR for the root cause.
     */
    IMU_ERROR_I2C_ERROR,

    /**
     * @brief Unexpected value of an I2c_ActionStatus return.
     */
    IMU_ERROR_INVALID_I2C_ACTION_STATUS

} Imu_ErrorCode;

/**
 * @brief The state of the top-level Imu component state machine.
 * 
 * These states indicated at a high level what the Imu component is doing. Most
 * of these states are also broken down into substates which allow the main
 * state to be broken down into a number of potentially blocking IO operations.
 * At each state the module step function can exit out waiting on IO operations
 * to complete, meaning that other modules are not blocked. 
 */
typedef enum _Imu_State {

    /**
     * @brief Set gyroscope offset state, which is the initial state the IMU
     * will be booted into.
     */
    IMU_STATE_SET_GYROSCOPE_OFFSETS = 0,

    /**
     * @brief Self test state.
     * 
     * TODO: may not be feasible due to requirements in [SELFTEST].
     */
    IMU_STATE_SELF_TEST,

    /**
     * @brief Waiting on new command state
     */
    IMU_STATE_WAIT_NEW_COMMAND,

    /**
     * @brief Read IMU temperature state.
     */
    IMU_STATE_READ_TEMPERATURE,

    /**
     * @brief Read IMU gyroscope data state.
     */
    IMU_STATE_READ_GYROSCOPE,

    /**
     * @brief Read IMU magnetometer data state.
     */
    IMU_STATE_READ_MAGNETOMETER,

    #ifdef F_IMU_CALIB
    /**
     * @brief Calibrate the gyroscope to read the propper offsets to use.
     * 
     * This state is only enabled when the `imu_calib` build feature is
     * enabled. 
     * 
     * In this state the user must ensure that the board/satellite is on a flat
     * surface and not moving. Many readings will be taken to calculate
     * offsets, and a certain precision is required.
     * 
     * The final calibration is output over the UART debug terminal, and must
     * be set subsequently in the configuration file.
     */
    IMU_STATE_CALIBRATE_GYROSCOPE,

    
    /** @brief Calibrate the magnetometer to read the propper offsets to use.
     * 
     * This state is only enabled when the `imu_calib` build feature is
     * enabled. 
     * 
     * In this state the user must ensure that the board/satellite is on a flat
     * surface and not moving. Many readings will be taken to calculate
     * offsets, and a certain precision is required.
     * 
     * The final calibration is output over the UART debug terminal, and must
     * subsequently be set in the configuration file.
     */
    IMU_STATE_CALIBRATE_MAGNETOMETER
    #endif

} Imu_State;

/**
 * @brief Substates of the Imu module state machine.
 * 
 * Each of these substates encapsulate a single IO operation which may block.
 */
typedef enum _Imu_SubState {

    /**
     * @brief No substate, which is considered invalid in states expecting to
     * be in a substate.
     */
    IMU_SUBSTATE_NONE = 0,

    /**
     * @brief Initial state for SET_GYROSCOPE_OFFSET, which sends the X values
     * to the IMU. 
     */
    IMU_SUBSTATE_SET_GYRO_OFFSET_INIT,

    /**
     * @brief Wait for gyroscope X offset values to be recieved successfully.
     */
    IMU_SUBSTATE_SET_GYRO_OFFSET_WAIT_X_COMPLETE,

    /**
     * @brief Wait for gyroscope Y offset values to be recieved successfully.
     */
    IMU_SUBSTATE_SET_GYRO_OFFSET_WAIT_Y_COMPLETE,

    /**
     * @brief Wait for gyroscope Z offset values to be recieved successfully.
     */
    IMU_SUBSTATE_SET_GYRO_OFFSET_WAIT_Z_COMPLETE,

    /**
     * @brief Initialise the read temperature state.
     */
    IMU_SUBSTATE_READ_TEMP_INIT,

    /**
     * @brief Waiting for the temperature read IO operation to complete.
     */
    IMU_SUBSTATE_READ_TEMP_WAIT_COMPLETE,

    /**
     * @brief Read Gyroscope X value state.
     */
    IMU_SUBSTATE_READ_GYROSCOPE_X,

    /**
     * @brief Read Gyroscope Y value state.
     */
    IMU_SUBSTATE_READ_GYROSCOPE_Y,

    /**
     * @brief Read Gyroscope Z value state.
     */
    IMU_SUBSTATE_READ_GYROSCOPE_Z,

    /**
     * @brief Read Magnetometer sensitivity adjust X value state.
     */
    IMU_SUBSTATE_READ_MAGNE_SENSE_ADJUST_X,

    /**
     * @brief Read Magnetometer sensitivity adjust Y value state.
     */
    IMU_SUBSTATE_READ_MAGNE_SENSE_ADJUST_Y,

    /**
     * @brief Read Magnetometer sensitivity adjust Z value state.
     */
    IMU_SUBSTATE_READ_MAGNE_SENSE_ADJUST_Z,

    /**
     * @brief Read Magnetometer X value state.
     */
    IMU_SUBSTATE_READ_MAGNE_X,

    /**
     * @brief Read Magnetometer Y value state.
     */
    IMU_SUBSTATE_READ_MAGNE_Y,

    /**
     * @brief Read Magnetometer Z value state.
     */
    IMU_SUBSTATE_READ_MAGNE_Z,

    /**
     * @brief Read Magnetometer ST2 state.
     * 
     * Unlike other magnetometer states this one does not save any data,
     * instead this is used to signal to the magnetometer that the end of the
     * data read has been completed.
     */
    IMU_SUBSTATE_READ_MAGNE_ST2

} Imu_SubState;

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Vector of 16 bit integers.
 */
typedef struct _Imu_VecInt16 {
    /**
     * @brief X component of the vector.
     */
    int16_t x;

    /**
     * @brief Y component of the vector.
     */
    int16_t y;

    /**
     * @brief Z component of the vector.
     */
    int16_t z;
} Imu_VecInt16;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the IMU component.
 * 
 * This function will initialise the hardware required to use the IMU 
 * component.
 * 
 * If this function returns false the associated error will be stored in 
 * DP.IMU.ERROR.
 * 
 * @return bool True on success, false on error.
 */
bool Imu_init(void);

/**
 * @brief Step the IMU component.
 * 
 * The step function will check for new IMU commands by polling for the
 * EVT_IMU_NEW_COMMAND event. If raised it will then execute the command in the
 * DP.IMU.COMMAND datapool parameter. 
 * 
 * If this function returns false the associated error will be stored in 
 * DP.IMU.ERROR.
 * 
 * @return bool True on success, false on error.
 */
bool Imu_step(void);

/**
 * @brief Issue a new command to the Imu component.
 * 
 * This function sets the command DataPool parameter and raises the new command
 * event.
 * 
 * If this function returns false the associated error will be stored in 
 * DP.IMU.ERROR.
 * 
 * @param command_in The command to execute.
 * @return true True on success, false on error.
 */
bool Imu_new_command(Imu_Command command_in);

#endif /* H_IMU_PUBLIC_H */

/** @} */ /* End of IMU group */