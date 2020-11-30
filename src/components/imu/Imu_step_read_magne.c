/**
 * @ingroup imu
 * @file Imu_step_read_magne.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Step the IMU component's READ_MAGNETOMETER state.
 * 
 * Task ref: [UT_2.10.1]
 * 
 * @version 0.1
 * @date 2020-11-23
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/data_pool/DataPool_public.h"
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/i2c/I2c_public.h"
#include "components/imu/Imu_public.h"
#include "components/imu/Imu_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Imu_step_read_magne(void) {
    /* No need to check for the module being initialised as this is done in
     * Imu_step() */

    /* Switch statement vars. Cases don't have separate scopes so variables
     * must be declared outside the switch */
    ErrorCode i2c_error = ERROR_NONE;
    bool i2c_action_finished = false;
    bool i2c_action_success = false;
    uint8_t sens_adjust_data = 0;
    uint8_t magne_data[2] = {0};
    uint8_t st2 = 0;

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
        case IMU_SUBSTATE_READ_MAGNE_INIT: ;

            /* Set the data valid flag to false */
            DP.IMU.MAGNETOMETER_DATA_VALID = false;

            /* Read bytes from the device */
            i2c_error = I2c_device_recv_bytes(
                &IMU_MAGNE_I2C_DEVICE, 
                IMU_REG_MAGNE_SENSE_ADJUST_X,
                1
            );
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while receiving gyroscope X: %d", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_READ_GYRO_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_READ_GYRO_FAILURE"
                    );
                }

                return false;
            }

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_READ_MAGNE_WAIT_SENSE_ADJUST_X_COMPLETE: ;

            /* Wait and trigger next read */
            if (!Imu_wait_i2c_read_complete(
                &IMU_MAGNE_I2C_DEVICE,
                &sens_adjust_data,
                &i2c_action_finished,
                EVT_IMU_READ_MAGNE_FAILURE,
                IMU_REG_MAGNE_SENSE_ADJUST_Y,
                1
            )) {
                /* Error code and message set by wait func */
                return false;
            }

            /* If not finished exit waiting on the I2C to succeed. This is not
             * an error though. */
            if (!i2c_action_finished) {
                return true;
            }

            /* Put the byte into the data pool field */
            DP.IMU.MAGNE_SENSE_ADJUST_DATA.x = sens_adjust_data;

            /* Advance to next substate */
            DP.IMU.SUBSTATE 
                = IMU_SUBSTATE_READ_MAGNE_WAIT_SENSE_ADJUST_Y_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_READ_MAGNE_WAIT_SENSE_ADJUST_Y_COMPLETE: ;

            /* Wait and trigger next read */
            if (!Imu_wait_i2c_read_complete(
                &IMU_MAGNE_I2C_DEVICE,
                &sens_adjust_data,
                &i2c_action_finished,
                EVT_IMU_READ_MAGNE_FAILURE,
                IMU_REG_MAGNE_SENSE_ADJUST_Z,
                1
            )) {
                /* Error code and message set by wait func */
                return false;
            }

            /* If not finished exit waiting on the I2C to succeed. This is not
             * an error though. */
            if (!i2c_action_finished) {
                return true;
            }

            /* Put the byte into the data pool field */
            DP.IMU.MAGNE_SENSE_ADJUST_DATA.y = sens_adjust_data;

            /* Advance to next substate */
            DP.IMU.SUBSTATE 
                = IMU_SUBSTATE_READ_MAGNE_WAIT_SENSE_ADJUST_Z_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_READ_MAGNE_WAIT_SENSE_ADJUST_Z_COMPLETE: ;

            /* Wait and trigger next read */
            if (!Imu_wait_i2c_read_complete(
                &IMU_MAGNE_I2C_DEVICE,
                &sens_adjust_data,
                &i2c_action_finished,
                EVT_IMU_READ_MAGNE_FAILURE,
                IMU_REG_MAGNE_X_OUT_L,
                2
            )) {
                /* Error code and message set by wait func */
                return false;
            }

            /* If not finished exit waiting on the I2C to succeed. This is not
             * an error though. */
            if (!i2c_action_finished) {
                return true;
            }

            /* Put the byte into the data pool field */
            DP.IMU.MAGNE_SENSE_ADJUST_DATA.z = sens_adjust_data;

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_READ_MAGNE_WAIT_X_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_READ_MAGNE_WAIT_X_COMPLETE: ;

            /* Wait and trigger next read */
            if (!Imu_wait_i2c_read_complete(
                &IMU_MAGNE_I2C_DEVICE,
                (uint8_t *)magne_data,
                &i2c_action_finished,
                EVT_IMU_READ_MAGNE_FAILURE,
                IMU_REG_MAGNE_Y_OUT_L,
                2
            )) {
                /* Error code and message set by wait func */
                return false;
            }

            /* If not finished exit waiting on the I2C to succeed. This is not
             * an error though. */
            if (!i2c_action_finished) {
                return true;
            }

            /* Put the bytes into the data pool field */
            /* TODO: Verify that the byte order here is correct */
            DP.IMU.MAGNETOMETER_DATA.x = (int16_t)(
                ((uint16_t)magne_data[1] << 8) /* Low byte is first */
                | (uint16_t)magne_data[0]
            );

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_READ_MAGNE_WAIT_Y_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_READ_MAGNE_WAIT_Y_COMPLETE: ;

            /* Wait and trigger next read */
            if (!Imu_wait_i2c_read_complete(
                &IMU_MAGNE_I2C_DEVICE,
                (uint8_t *)magne_data,
                &i2c_action_finished,
                EVT_IMU_READ_MAGNE_FAILURE,
                IMU_REG_MAGNE_Z_OUT_L,
                2
            )) {
                /* Error code and message set by wait func */
                return false;
            }

            /* If not finished exit waiting on the I2C to succeed. This is not
             * an error though. */
            if (!i2c_action_finished) {
                return true;
            }

            /* Put the bytes into the data pool field */
            /* TODO: Verify that the byte order here is correct */
            DP.IMU.MAGNETOMETER_DATA.y = (int16_t)(
                ((uint16_t)magne_data[1] << 8) /* Low byte is first */
                | (uint16_t)magne_data[0]
            );

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_READ_MAGNE_WAIT_Z_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_READ_MAGNE_WAIT_Z_COMPLETE: ;

            /* Wait and trigger next read */
            if (!Imu_wait_i2c_read_complete(
                &IMU_MAGNE_I2C_DEVICE,
                (uint8_t *)magne_data,
                &i2c_action_finished,
                EVT_IMU_READ_MAGNE_FAILURE,
                IMU_REG_MAGNE_ST2,
                1
            )) {
                /* Error code and message set by wait func */
                return false;
            }

            /* If not finished exit waiting on the I2C to succeed. This is not
             * an error though. */
            if (!i2c_action_finished) {
                return true;
            }

            /* Put the bytes into the data pool field */
            /* TODO: Verify that the byte order here is correct */
            DP.IMU.MAGNETOMETER_DATA.z = (int16_t)(
                ((uint16_t)magne_data[1] << 8) /* Low byte is first */
                | (uint16_t)magne_data[0]
            );

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_READ_MAGNE_WAIT_ST2_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_READ_MAGNE_WAIT_ST2_COMPLETE: ;

            /* Wait for the I2c operation to finish */
            if (!Imu_wait_i2c_action_finished(
                &IMU_MAGNE_I2C_DEVICE,
                &i2c_action_finished,
                &i2c_action_success,
                EVT_IMU_READ_MAGNE_FAILURE
            )) {
                /* Imu_wait_i2c_action_finished will set error code and 
                 * message */
                return false;
            }

            /* If not finished continue waiting for the IO to complete */
            if (!i2c_action_finished) {
                return true;
            }

            /* If it's finished but not successful exit */
            if (!i2c_action_success) {
                /* Error code and message set by wait func */
                return false;
            }

            /* If it was successful read the bytes from the I2C driver */
            i2c_error = I2c_get_device_recved_bytes(
                &IMU_MAGNE_I2C_DEVICE,
                &st2
            );
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while reading magnetometer ST2 byte: %d", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_READ_MAGNE_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_READ_MAGNE_FAILURE"
                    );
                }

                return false;
            }

            /* Remove the action */
            i2c_error = I2c_clear_device_action(&IMU_MAIN_I2C_DEVICE);
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while clearing read magne ST2 action: %d", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_READ_MAGNE_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_READ_MAGNE_FAILURE"
                    );
                }

                return false;
            }

            DEBUG_DBG(
                "Raw magne values: (%d, %d, %d)",
                DP.IMU.MAGNETOMETER_DATA.x,
                DP.IMU.MAGNETOMETER_DATA.y,
                DP.IMU.MAGNETOMETER_DATA.z
            );

            /* Calculate the adjusted magnetometer readings, as per the formula
             * in [REGMAP] p. 53 */
            double x_multiplier 
                = ((((DP.IMU.MAGNE_SENSE_ADJUST_DATA.x - 128)*0.5)/128) + 1);
            DP.IMU.MAGNETOMETER_DATA.x 
                = (int16_t)((double)DP.IMU.MAGNETOMETER_DATA.x * x_multiplier);

            double y_multiplier 
                = ((((DP.IMU.MAGNE_SENSE_ADJUST_DATA.y - 128)*0.5)/128) + 1);
            DP.IMU.MAGNETOMETER_DATA.y 
                = (int16_t)((double)DP.IMU.MAGNETOMETER_DATA.y * y_multiplier);

            double z_multiplier 
                = ((((DP.IMU.MAGNE_SENSE_ADJUST_DATA.z - 128)*0.5)/128) + 1);
            DP.IMU.MAGNETOMETER_DATA.z 
                = (int16_t)((double)DP.IMU.MAGNETOMETER_DATA.z * z_multiplier);

            DEBUG_DBG(
                "Magne multipliers: (%.6f, %.6f, %.6f)",
                x_multiplier,
                y_multiplier,
                z_multiplier
            );
            DEBUG_DBG(
                "Adjusted magne values: (%d, %d, %d)",
                DP.IMU.MAGNETOMETER_DATA.x,
                DP.IMU.MAGNETOMETER_DATA.y,
                DP.IMU.MAGNETOMETER_DATA.z
            );

            /* Set the data valid flag to true */
            DP.IMU.MAGNETOMETER_DATA_VALID = true;

            /* Raise success event */
            if (!EventManager_raise_event(EVT_IMU_READ_MAGNE_SUCCESS)) {
                DEBUG_ERR("Error raising EVT_IMU_READ_MAGNE_SUCCESS");
                DP.IMU.ERROR_CODE = IMU_ERROR_EVENTMANAGER_ERROR;
                return false;
            }

            /* Begin next state, new command */
            if (!Imu_begin_state(
                IMU_STATE_WAIT_NEW_COMMAND,
                IMU_SUBSTATE_NONE
            )) {
                DEBUG_ERR("Error begining IMU_STATE_WAIT_NEW_COMMAND");
                DP.IMU.ERROR_CODE = IMU_ERROR_EVENTMANAGER_ERROR;
                return false;
            }

            /* End of standard state machine, stop here */
            break;
        default:
            DEBUG_ERR("Invalid DP.IMU.SUBSTATE value: %d", DP.IMU.SUBSTATE);
            DP.IMU.ERROR_CODE = IMU_ERROR_INVALID_SUBSTATE;
            return false;
    }
    #pragma GCC diagnostic pop

    return true;
}