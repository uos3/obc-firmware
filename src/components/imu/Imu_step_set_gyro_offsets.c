/**
 * @ingroup imu
 * @file Imu_step_set_gyro_offsets.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Implementation of the Imu_step_set_gyro_offsets function.
 * 
 * @version 0.1
 * @date 2020-11-20
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/i2c/I2c_public.h"
#include "components/imu/Imu_public.h"
#include "components/imu/Imu_private.h"

bool Imu_step_set_gyro_offsets(void) {
    /* No need to check for the module being initialised as this is done in
     * Imu_step() */

    /* TODO: Offsets shall be stored in the config, for testing we use dummy
     * easily recognisable values. The first byte is the IMU register, next two
     * are the high and low bytes respectively. */
    uint8_t offset_x[3] = {IMU_REG_GYRO_X_OFFSET_H, 0x12, 0x34};
    uint8_t offset_y[3] = {IMU_REG_GYRO_Y_OFFSET_H, 0x56, 0x78};
    uint8_t offset_z[3] = {IMU_REG_GYRO_Z_OFFSET_H, 0x9A, 0xBC};

    /* Switch statement vars. Cases don't have separate scopes so variables
     * must be declared outside the switch */
    I2c_ErrorCode i2c_error = I2C_ERROR_NONE;
    bool is_event_raised = false;
    bool i2c_action_finished = false;
    bool i2c_action_success = false;

    /* 
     * Execute substate specific steps. 
     * 
     * The general idea with these substates is that there's a break where we
     * wait for IO. This means that the first part of a substate (except the
     * init) is waiting on an I2C event. Once the I2C event is fired we then
     * either continue to the next IO operation or exit on an error.
     *
     * Also disable switch-enum warning as this causes a warning for each 
     * missed IMU_SUBSTATE, but this is handled nicely by the default here. 
     */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (DP.IMU.SUBSTATE) {
        case IMU_SUBSTATE_SET_GYRO_OFFSET_INIT:

            /* Send X offset bytes to the device */
            i2c_error = I2c_device_send_bytes(
                &IMU_I2C_DEVICE, 
                (uint8_t *)offset_x, 
                3
            );
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while setting gyroscope X offsets: %d", 
                    i2c_error
                );
                DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_SET_GYRO_OFFSETS_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_SET_GYRO_OFFSETS_FAILURE"
                    );
                }

                return false;
            }

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_SET_GYRO_OFFSET_WAIT_X_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_SET_GYRO_OFFSET_WAIT_X_COMPLETE:

            /* Wait for the send action to complete */
            if (!Imu_wait_i2c_action_finished(
                &i2c_action_finished,
                &i2c_action_success,
                EVT_IMU_SET_GYRO_OFFSETS_FAILURE
            )) {
                /* Error code and message handled by wait function */
                return false;
            }

            /* If the action is not finished return true, waiting on IO */
            if (!i2c_action_finished) {
                return true;
            }

            /* If the action is finished but was not successful exit with an
             * error. There's no need to raise the failure event as this is
             * done by Imu_wait_i2c_action_finished, as well as the error code
             * and log messages. */
            if (!i2c_action_success) {
                return false;
            }

            /* Clear the action on the device */
            i2c_error = I2c_clear_device_action(&IMU_I2C_DEVICE);
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR("Could not clear action on IMU_I2C_DEVICE");
                DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
                return false;
            };

            /* The send was successful, send Y offset bytes to the device */
            i2c_error = I2c_device_send_bytes(
                &IMU_I2C_DEVICE, 
                (uint8_t *)offset_y, 
                3
            );
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while setting gyroscope Y offsets: %d", 
                    i2c_error
                );
                DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_SET_GYRO_OFFSETS_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_SET_GYRO_OFFSETS_FAILURE"
                    );
                }

                return false;
            }

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_SET_GYRO_OFFSET_WAIT_Y_COMPLETE;


            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_SET_GYRO_OFFSET_WAIT_Y_COMPLETE:

            /* Wait for the send action to complete */
            if (!Imu_wait_i2c_action_finished(
                &i2c_action_finished,
                &i2c_action_success,
                EVT_IMU_SET_GYRO_OFFSETS_FAILURE
            )) {
                /* Error code and message handled by wait function */
                return false;
            }

            /* If the action is not finished return true, waiting on IO */
            if (!i2c_action_finished) {
                return true;
            }

            /* If the action is finished but was not successful exit with an
             * error. There's no need to raise the failure event as this is
             * done by Imu_wait_i2c_action_finished, as well as the error code
             * and log messages. */
            if (!i2c_action_success) {
                return false;
            }

            /* Clear the action on the device */
            i2c_error = I2c_clear_device_action(&IMU_I2C_DEVICE);
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR("Could not clear action on IMU_I2C_DEVICE");
                DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
                return false;
            };

            /* Send Y offset bytes to the device */
            i2c_error = I2c_device_send_bytes(
                &IMU_I2C_DEVICE, 
                (uint8_t *)offset_z, 
                3
            );
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while setting gyroscope Z offsets: %d", 
                    i2c_error
                );
                DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_SET_GYRO_OFFSETS_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_SET_GYRO_OFFSETS_FAILURE"
                    );
                }

                return false;
            }

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_SET_GYRO_OFFSET_WAIT_Z_COMPLETE:

            /* Wait for the send action to complete */
            if (!Imu_wait_i2c_action_finished(
                &i2c_action_finished,
                &i2c_action_success,
                EVT_IMU_SET_GYRO_OFFSETS_FAILURE
            )) {
                /* Error code and message handled by wait function */
                return false;
            }

            /* If the action is not finished return true, waiting on IO */
            if (!i2c_action_finished) {
                return true;
            }

            /* If the action is finished but was not successful exit with an
             * error. There's no need to raise the failure event as this is
             * done by Imu_wait_i2c_action_finished, as well as the error code
             * and log messages. */
            if (!i2c_action_success) {
                return false;
            }

            /* Clear the action on the device */
            i2c_error = I2c_clear_device_action(&IMU_I2C_DEVICE);
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR("Could not clear action on IMU_I2C_DEVICE");
                DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
                return false;
            };

            /* The send was successful, raise the success event */
            if (!EventManager_raise_event(EVT_IMU_SET_GYRO_OFFSETS_SUCCESS)) {
                DEBUG_ERR("Error raising EVT_IMU_SET_GYRO_OFFSETS_SUCCESS");
                DP.IMU.ERROR = IMU_ERROR_EVENTMANAGER_ERROR;
                return false;
            }

            /* Begin the next state, which shall always be wait new command */
            if (!Imu_begin_state(
                IMU_STATE_WAIT_NEW_COMMAND,
                IMU_SUBSTATE_NONE
            )) {
                /* No need to set error codes as Imu_begin_state does this */
                return false;
            }

            /* End of the state, break out of the switch to return true */

            break;
        default:
            DEBUG_ERR(
                "Invalid substate for SET_GYROSCOPE_OFFSETS state: %d", 
                DP.IMU.SUBSTATE
            );
            DP.IMU.ERROR = IMU_ERROR_INVALID_SUBSTATE;
            return false;
    }
    #pragma GCC diagnostic pop

    return true;
}