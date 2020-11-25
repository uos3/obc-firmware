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
 * @brief The I2C device which represents the main IMU.
 * 
 * The main IMU is connected on module I2C2, with an address of 0x68.
 * 
 * The main device contains the gyroscope, accelerometer, and temperature
 * sensors. The magnetometer is found on a different device, even though they
 * are in the same physical chip.
 */
extern I2c_Device IMU_MAIN_I2C_DEVICE;

/**
 * @brief The I2C device which represents the IMU's magnetometer.
 * 
 * The IMU magnetometer is connected on module I2C2, with an address of 0x0C.
 * 
 * The magnetometer is actually a separate device inside the IMU, and has a
 * separate register space and I2C address.
 */
extern I2c_Device IMU_MAGNE_I2C_DEVICE;

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief IMU register address for the INT pin / bypass enable config.
 * 
 * From [REGMAP] p. 8.
 */
#define IMU_REG_INT_PIN_CFG (0x37)

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
 * @brief IMU register address for Gyroscope X output high byte.
 * 
 * From [REGMAP] p. 8.
 */
#define IMU_REG_GYRO_X_OUT_H (0x43)

/**
 * @brief IMU register address for Gyroscope Y output high byte.
 * 
 * From [REGMAP] p. 8.
 */
#define IMU_REG_GYRO_Y_OUT_H (0x45)

/**
 * @brief IMU register address for Gyroscope Z output high byte.
 * 
 * From [REGMAP] p. 8.
 */
#define IMU_REG_GYRO_Z_OUT_H (0x47)

/**
 * @brief IMU register address for magnetometer X sensitivity adjust.
 * 
 * From [REGMAP] p. 47
 */
#define IMU_REG_MAGNE_SENSE_ADJUST_X (0x10)

/**
 * @brief IMU register address for magnetometer X sensitivity adjust.
 * 
 * From [REGMAP] p. 47
 */
#define IMU_REG_MAGNE_SENSE_ADJUST_Y (0x11)

/**
 * @brief IMU register address for magnetometer X sensitivity adjust.
 * 
 * From [REGMAP] p. 47
 */
#define IMU_REG_MAGNE_SENSE_ADJUST_Z (0x12)

/**
 * @brief IMU register address for magnetometer X value.
 * 
 * From [REGMAP] p. 47
 */
#define IMU_REG_MAGNE_X_OUT_L (0x03)

/**
 * @brief IMU register address for magnetometer Y value.
 * 
 * From [REGMAP] p. 47
 */
#define IMU_REG_MAGNE_Y_OUT_L (0x05)

/**
 * @brief IMU register address for magnetometer Z value.
 * 
 * From [REGMAP] p. 47
 */
#define IMU_REG_MAGNE_Z_OUT_L (0x07)

/**
 * @brief IMU register address for magnetometer Status 2 register.
 * 
 * From [REGMAP] p. 47
 */
#define IMU_REG_MAGNE_ST2 (0x09)

/**
 * @brief IMU register address for the temperature out high byte.
 * 
 * From [REGMAP] p. 8.
 */
#define IMU_REG_TEMP_OUT (0x41)

/**
 * @brief Bypass enable bit of the INT_PIN_CFG register is bit1.
 *
 * From [REGMAP] p. 8. 
 */
#define IMU_INT_PIN_CFG_BYPASS_ENABLE_BIT (0b10)

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
 * @param p_device_in Pointer to the I2C device to operate on.
 * @param p_finished_out True if the action finished.
 * @param p_success_out True if the action was successful.
 * @param failure_event The event to raise if the action was a failure.
 * @return true True on function (not action) success, false on error.
 */
bool Imu_wait_i2c_action_finished(
    I2c_Device *p_device_in,
    bool *p_finished_out, 
    bool *p_success_out,
    Event failure_event
);

/**
 * @brief Wait for a read I2C operation to complete as a part of a multi-send
 * action. 
 * 
 * The length of p_data_out should be the same as that set in the previous
 * I2c_device_read_bytes call (or this function).
 * 
 * @param p_device_in Pointer to the I2C device to operate on.
 * @param p_data_out Pointer to a byte array of the proper length which the
 *        recieved data will be set into.
 * @param p_finished_out True if the operation has finished and the next I2C
 *        read action has been started.
 * @param evt_failure_in Event to raise if a failure occurs.
 * @param next_i2c_read_reg_in The register address of the next I2C read action.
 * @param data_length_in The length of the next data recieve command.
 * @return bool True if no error occurs, false otherwise.
 */
bool Imu_wait_i2c_read_complete(
    I2c_Device *p_device_in,
    uint8_t *p_data_out,
    bool *p_finished_out,
    Event evt_failure_in,
    uint8_t next_i2c_read_reg_in,
    size_t next_i2c_read_length_in
);


#endif /* H_IMU_PRIVATE_H */