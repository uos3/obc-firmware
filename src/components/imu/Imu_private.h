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
#include "system/event_manager/EventManager_public.h"
#include "drivers/i2c/I2c_public.h"
#include "components/imu/Imu_public.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief The I2C device which represents the IMU.
 * 
 * The IMU is connected on module I2C2, with an address of 0x68.
 */
extern I2c_Device IMU_I2C_DEVICE;

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief IMU register address for the Gyroscope X offset high byte. 
 * 
 * From [REGMAP] p. 8.
 */
#define IMU_REG_GYRO_X_OFFSET_H (0x13)

/**
 * @brief IMU register address for the Gyroscope X offset high byte. 
 * 
 * From [REGMAP] p. 8.
 */
#define IMU_REG_GYRO_Y_OFFSET_H (0x15)

/**
 * @brief IMU register address for the Gyroscope X offset high byte. 
 * 
 * From [REGMAP] p. 8.
 */
#define IMU_REG_GYRO_Z_OFFSET_H (0x17)

/**
 * @brief IMU register address for the temperature out high byte.
 * 
 * From [REGMAP] p. 8.
 */
#define IMU_REG_TEMP_OUT (0x41)

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Setup the Imu module to begin executing the given state in the next
 * cycle. 
 * 
 * The state and initial substate shall be set, followed by the raising of the
 * EVT_IMU_STATE_CHANGE event, which will cause the system to not go to sleep,
 * therefore ensuring the next cycle occures immediately rather than waiting
 * for an interrupt.
 * 
 * @param state The state to begin.
 * @param initial_substate The initial substate that the module should be in
 *        upon starting the new state.
 * @return bool True on success, false on failure.
 */
bool Imu_begin_state(Imu_State state, Imu_SubState initial_substate);

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

/**
 * @brief Wait for an I2C IO action to finish
 * 
 * @param p_finished_out True if the action finished.
 * @param p_success_out True if the action was successful.
 * @param failure_event The event to raise if the action was a failure.
 * @return true True on function (not action) success, false on error.
 */
bool Imu_wait_i2c_action_finished(
    bool *p_finished_out, 
    bool *p_success_out,
    Event failure_event
);


#endif /* H_IMU_PRIVATE_H */