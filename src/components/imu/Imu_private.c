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

I2c_Device IMU_MAIN_I2C_DEVICE = {
    0x2,
    0x68
};

I2c_Device IMU_MAGNE_I2C_DEVICE = {
    0x2,
    0x0C
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

bool Imu_wait_i2c_action_finished(
    I2c_Device *p_device_in,
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
            p_device_in,
            &i2c_status
        );
        if (i2c_error != I2C_ERROR_NONE) {
            DEBUG_ERR("Error getting I2C action status for IMU device");
            DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
            DP.IMU.I2C_ERROR = i2c_error;

            /* Raise the failed event */
            if (!EventManager_raise_event(
                failure_event
            )) {
                DEBUG_ERR(
                    "CRITICAL: could not raise event 0x%04X",
                    failure_event
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
                p_device_in,
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
                    "CRITICAL: could not raise event 0x%04X",
                    (uint16_t)failure_event
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

bool Imu_wait_i2c_read_complete(
    I2c_Device *p_device_in,
    uint8_t *p_data_out,
    bool *p_finished_out,
    Event evt_failure_in,
    uint8_t next_i2c_read_reg_in,
    size_t next_i2c_read_length_in
) {
    /* Variables used throughout this function */
    bool i2c_action_finished = false;
    bool i2c_action_success = false;
    I2c_ErrorCode i2c_error;

    /* Default finished to false */
    *p_finished_out = false;

    /* Wait for the I2c operation to finish */
    if (!Imu_wait_i2c_action_finished(
        p_device_in,
        &i2c_action_finished,
        &i2c_action_success,
        evt_failure_in
    )) {
        /* Imu_wait_i2c_action_finished will set error code and 
            * message */
        return false;
    }

    /* If not finished continue waiting for the IO to complete */
    if (!i2c_action_finished) {
        DEBUG_TRC(
            "Waiting for I2C action to finish in IMU substate 0x%02X",
            DP.IMU.SUBSTATE
        );
        return true;
    }

    /* If it's finished but not successful exit */
    if (!i2c_action_success) {
        /* Error code and message set by wait func */
        return false;
    }

    /* If it was successful read the bytes from the I2C driver */
    i2c_error = I2c_get_device_recved_bytes(
        p_device_in,
        p_data_out
    );
    if (i2c_error != I2C_ERROR_NONE) {
        DEBUG_ERR(
            "I2C error while reading bytes: %d, DP.IMU.SUBSTATE = 0x%02X", 
            i2c_error,
            DP.IMU.SUBSTATE
        );
        DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
        DP.IMU.I2C_ERROR = i2c_error;

        /* Raise the failed event */
        if (!EventManager_raise_event(
            evt_failure_in
        )) {
            DEBUG_ERR(
                "CRITICAL: could not raise IMU failure event 0x%04X",
                (uint16_t)evt_failure_in
            );
        }

        return false;
    }

    /* Remove the action */
    i2c_error = I2c_clear_device_action(p_device_in);
    if (i2c_error != I2C_ERROR_NONE) {
        DEBUG_ERR(
            "I2C error while clearing action: %d, DP.IMU.SUBSTATE = 0x%02X", 
            i2c_error,
            DP.IMU.SUBSTATE
        );
        DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
        DP.IMU.I2C_ERROR = i2c_error;

        /* Raise the failed event */
        if (!EventManager_raise_event(
            evt_failure_in
        )) {
            DEBUG_ERR(
                "CRITICAL: could not raise IMU failure event 0x%04X",
                (uint16_t)evt_failure_in
            );
        }

        return false;
    }

    /* Read bytes from the device */
    i2c_error = I2c_device_recv_bytes(
        p_device_in, 
        next_i2c_read_reg_in,
        next_i2c_read_length_in
    );
    if (i2c_error != I2C_ERROR_NONE) {
        DEBUG_ERR(
            "I2C error while requesting recv: %d, DP.IMU.SUBSTATE = 0x%02X", 
            i2c_error,
            DP.IMU.SUBSTATE
        );
        DP.IMU.ERROR = IMU_ERROR_I2C_ERROR;
        DP.IMU.I2C_ERROR = i2c_error;

        /* Raise the failed event */
        if (!EventManager_raise_event(
            evt_failure_in
        )) {
            DEBUG_ERR(
                "CRITICAL: could not raise IMU failure event 0x%04X",
                (uint16_t)evt_failure_in
            );
        }

        return false;
    }

    /* Raise the finished flag */
    *p_finished_out = true;

    return true;
}