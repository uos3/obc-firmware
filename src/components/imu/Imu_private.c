/**
 * @ingroup imu
 * @file Imu_private.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief IMU component private source, see corresponding header file for more 
 * information.
 * 
 * Task ref: [UT_2.10.1]
 * 
 * @version 0.1
 * @date 2020-11-17
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */

/* Internal includes */
#include "system/data_pool/DataPool_public.h"
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/i2c/I2c_public.h"
#include "components/imu/Imu_public.h"
#include "components/imu/Imu_private.h"

/* -------------------------------------------------------------------------   
 * CONSTANTS
 * ------------------------------------------------------------------------- */

I2c_Device IMU_I2C_DEVICE = {
    0x2,
    0x68
};

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Imu_begin_state(Imu_State state, Imu_SubState initial_substate) {
    /* Set the state and subsate */
    DP.IMU.STATE = state;
    DP.IMU.SUBSTATE = initial_substate;

    /* Raise the state change event, which will ensure that the system does not
     * go to sleep and immediately executes the next cycle */
    if (!EventManager_raise_event(EVT_IMU_STATE_CHANGE)) {
        DEBUG_ERR("Error rasising EVT_IMU_STATE_CHANGE event");
        DP.IMU.ERROR = IMU_ERROR_EVENTMANAGER_ERROR;
        return false;
    }

    return true;
}

bool Imu_step_self_test(void) {
    // TODO
    DEBUG_WRN("Self test not yet implemented");
    return true;
}

bool Imu_step_read_temp(void) {
    /* No need to check for the module being initialised as this is done in
     * Imu_step() */

    /* Switch statement vars. Cases don't have separate scopes so variables
     * must be declared outside the switch */
    I2c_ErrorCode i2c_error = I2C_ERROR_NONE;
    bool is_event_raised = false;
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
                &IMU_I2C_DEVICE, 
                IMU_REG_TEMP_OUT,
                2
            );
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while receiving temperature: %d", 
                    i2c_error
                );
                DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR = i2c_error;

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

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_READ_TEMP_WAIT_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_READ_TEMP_WAIT_COMPLETE: ;

            /* Wait for the I2c operation to finish */
            if (!Imu_wait_i2c_action_finished(
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
                &IMU_I2C_DEVICE,
                (uint8_t *)temp_data
            );
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while reading temp bytes: %d", 
                    i2c_error
                );
                DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR = i2c_error;

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
            DP.IMU.TEMPERATURE_DATA = (int16_t)(
                ((uint16_t)temp_data[0] << 8) /* High byte is first */
                | (uint16_t)temp_data[1]
            );

            /* Remove the action */
            i2c_error = I2c_clear_device_action(&IMU_I2C_DEVICE);
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while clearing read temp action: %d", 
                    i2c_error
                );
                DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR = i2c_error;

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

            /* Raise success event */
            if (!EventManager_raise_event(EVT_IMU_READ_TEMP_SUCCESS)) {
                DEBUG_ERR("Error raising EVT_IMU_READ_TEMP_SUCCESS");
                DP.IMU.ERROR = IMU_ERROR_EVENTMANAGER_ERROR;
                return false;
            }

            break;
        default: ;
            DEBUG_ERR(
                "Invalid IMU substate for READ_TEMPERATURE state: %d",
                DP.IMU.SUBSTATE  
            );
            DP.IMU.ERROR = IMU_ERROR_INVALID_SUBSTATE;
            return false;
    }
    #pragma GCC diagnostic pop

    return true;
}

bool Imu_step_read_gyro(void) {
    // TODO
    return true;
}

bool Imu_step_read_magne(void) {
    // TODO
    return true;
}

bool Imu_wait_i2c_action_finished(
    bool *p_finished_out, 
    bool *p_success_out,
    Event failure_event
) {

    /* Assign default values for outputs */
    *p_finished_out = false;
    *p_success_out = false;
    
    /* Get I2C finished event status */
    bool is_event_raised = false;
    if (!EventManager_poll_event(
        EVT_I2C_ACTION_FINISHED, 
        &is_event_raised
    )) {
        DEBUG_ERR("Error reading EVT_I2C_ACTION_FINISHED event");
        DP.IMU.ERROR = IMU_ERROR_EVENTMANAGER_ERROR;

        /* Raise the failed event */
        if (!EventManager_raise_event(
            failure_event
        )) {
            DEBUG_ERR(
                "CRITICAL: could not raise IMU failure event"
            );
        }

        return false;
    }

    /* If not raised exit and wait for IO to complete */
    if (!is_event_raised) {
        DEBUG_TRC("Waiting for EVT_I2C_ACTION_FINISHED");
        return true;
    }
    /* Otherwise set finished to true and calculate success */
    else {
        *p_finished_out = true;

        /* Get the status of the operation */
        I2c_ErrorCode i2c_error = I2C_ERROR_NONE;
        I2c_ActionStatus i2c_status = I2C_ACTION_STATUS_NO_ACTION;
        i2c_error = I2c_get_device_action_status(
            &IMU_I2C_DEVICE,
            &i2c_status
        );
        if (i2c_error != I2C_ERROR_NONE) {
            DEBUG_ERR("Error getting I2C action status for IMU device");
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

        /* If the operation was successful */
        if (i2c_status == I2C_ACTION_STATUS_SUCCESS) {
            *p_success_out = true;
        }
        /* If it failed */
        else if (i2c_status == I2C_ACTION_STATUS_FAILURE) {

            /* No need to set success as false as this is the default value */
            
            DEBUG_ERR("I2C action failed");
            DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;

            /* Get the error cause */
            I2c_ErrorCode i2c_action_error = I2C_ERROR_NONE;
            i2c_error = I2c_get_device_action_failure_cause(
                &IMU_I2C_DEVICE,
                &i2c_action_error
            );
            if (i2c_error != I2C_ERROR_NONE) {
                DEBUG_ERR(
                    "CRITICAL: could not get I2C action error cause"
                );
                return false;
            }

            DP.IMU.I2C_ERROR = i2c_action_error;

            /* Raise the failed event */
            if (!EventManager_raise_event(
                failure_event
            )) {
                DEBUG_ERR(
                    "CRITICAL: could not raise EVT_IMU_SET_GYRO_OFFSETS_FAILURE"
                );
                return false;
            }
        }
        /* If something else is happening */
        else {
            DEBUG_ERR(
                "Unexpected I2c_ActionStatus value for finished operation: %d", 
                i2c_status
            );
            DP.IMU.ERROR = IMU_ERROR_INVALID_I2C_ACTION_STATUS;
            return false;
        }
    }

    return true;
}