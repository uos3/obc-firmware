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
    return true;
}

bool Imu_step_read_temp(void) {
    // TODO
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