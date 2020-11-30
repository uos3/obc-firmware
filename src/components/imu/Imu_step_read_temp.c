/**
 * @ingroup imu
 * @file Imu_step_read_temp.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Step the IMU component's READ_TEMPERATURE state.
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

bool Imu_step_read_temp(void) {
    /* No need to check for the module being initialised as this is done in
     * Imu_step() */

    /* Switch statement vars. Cases don't have separate scopes so variables
     * must be declared outside the switch */
    ErrorCode i2c_error = ERROR_NONE;
    bool i2c_action_finished = false;
    bool i2c_action_success = false;
    uint8_t temp_data[2] = {0};
    
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
        case IMU_SUBSTATE_READ_TEMP_INIT: ;

            /* Read bytes from the device */
            i2c_error = I2c_device_recv_bytes(
                &IMU_MAIN_I2C_DEVICE, 
                IMU_REG_TEMP_OUT,
                2
            );
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while receiving temperature: %d", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_READ_TEMP_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_READ_TEMP_FAILURE"
                    );
                }

                return false;
            }

            /* Set the valid flag to false */
            DP.IMU.TEMPERATURE_DATA_VALID = false;

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_READ_TEMP_WAIT_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_READ_TEMP_WAIT_COMPLETE: ;

            /* Wait for the I2c operation to finish */
            if (!Imu_wait_i2c_action_finished(
                &IMU_MAIN_I2C_DEVICE,
                &i2c_action_finished,
                &i2c_action_success,
                EVT_IMU_READ_TEMP_FAILURE
            )) {
                /* Imu_wait_i2c_action_finished will set error code and 
                 * message */
                return false;
            }

            /* If not finished continue waiting for the IO to complete */
            if (!i2c_action_finished) {
                DEBUG_TRC("Waiting for I2C action to finish in READ_TEMP");
                return true;
            }

            /* If it's finished but not successful exit */
            if (!i2c_action_success) {
                /* Error code and message set by wait func */
                return false;
            }

            /* If it was successful read the bytes from the I2C driver */
            i2c_error = I2c_get_device_recved_bytes(
                &IMU_MAIN_I2C_DEVICE,
                (uint8_t *)temp_data
            );
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while reading temp bytes: %d", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_READ_TEMP_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_READ_TEMP_FAILURE"
                    );
                }

                return false;
            }

            /* Put the bytes into the data pool field */
            /* TODO: Verify that the byte order here is correct */
            DP.IMU.TEMPERATURE_DATA = (int16_t)(
                ((uint16_t)temp_data[0] << 8) /* High byte is first */
                | (uint16_t)temp_data[1]
            );

            /* Remove the action */
            i2c_error = I2c_clear_device_action(&IMU_MAIN_I2C_DEVICE);
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while clearing read temp action: %d", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_READ_TEMP_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_READ_TEMP_FAILURE"
                    );
                }

                return false;
            }

            /* Set the valid flag to true */
            DP.IMU.TEMPERATURE_DATA_VALID = true;

            /* Raise success event */
            if (!EventManager_raise_event(EVT_IMU_READ_TEMP_SUCCESS)) {
                DEBUG_ERR("Error raising EVT_IMU_READ_TEMP_SUCCESS");
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

            break;
        default: ;
            DEBUG_ERR(
                "Invalid IMU substate for READ_TEMPERATURE state: %d",
                DP.IMU.SUBSTATE  
            );
            DP.IMU.ERROR_CODE = IMU_ERROR_INVALID_SUBSTATE;
            return false;
    }
    #pragma GCC diagnostic pop

    return true;
}