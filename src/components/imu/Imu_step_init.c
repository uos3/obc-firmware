/**
 * @ingroup imu
 * @file Imu_step_init.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Step function for the initialisation mode.
 * 
 * Task ref: [UT_2.10.1]
 * 
 * @version 0.1
 * @date 2020-12-03
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


bool Imu_step_init(void) {
    /* No need to check for the module being initialised as this is done in
     * Imu_step() */

    /* Switch statement vars. Cases don't have separate scopes so variables
     * must be declared outside the switch */
    ErrorCode i2c_error = ERROR_NONE;
    bool i2c_action_finished = false;
    bool i2c_action_success = false;
    uint8_t int_pin_cfg = 0;

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

        case IMU_SUBSTATE_INIT_INIT: ;

            /* Begin the read of the IMU_REG_INT_PIN_CFG register */
            i2c_error = I2c_device_recv_bytes(
                &IMU_MAIN_I2C_DEVICE,
                IMU_REG_INT_PIN_CFG,
                1
            );
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while receiving IMU_REG_INT_PIN_CFG: 0x%04X", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_INIT_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_INIT_FAILURE"
                    );
                }

                return false;
            }

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_INIT_WAIT_INT_PIN_CFG_READ_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_INIT_WAIT_INT_PIN_CFG_READ_COMPLETE: ;

            /* Wait for the I2c operation to finish */
            if (!Imu_wait_i2c_action_finished(
                &IMU_MAIN_I2C_DEVICE,
                &i2c_action_finished,
                &i2c_action_success,
                EVT_IMU_INIT_FAILURE
            )) {
                /* Imu_wait_i2c_action_finished will set error code and 
                 * message */
                return false;
            }

            /* If the operation isn't finished return */
            if (!i2c_action_finished) {
                return true;
            }

            /* Otherwise check for action failure */
            if (!i2c_action_success) {
                /* The wait function will set the appropriate error messages */
                return false;
            }

            /* Get the byte read from the device */
            i2c_error = I2c_get_device_recved_bytes(
                &IMU_MAIN_I2C_DEVICE,
                &int_pin_cfg
            );
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while reading INT_PIN_CFG from IMU: 0x%04X", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_INIT_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise IMU failure event 0x%04X",
                        EVT_IMU_INIT_FAILURE
                    );
                }

                /* Clear the I2C action */
                i2c_error = I2c_clear_device_action(&IMU_MAIN_I2C_DEVICE);
                if (i2c_error != ERROR_NONE) {
                    DEBUG_ERR(
                        "CRITICAL: could not clear the I2C device action"
                    );
                }

                return false;
            }

            /* Clear the I2C action */
            i2c_error = I2c_clear_device_action(&IMU_MAIN_I2C_DEVICE);
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while reading INT_PIN_CFG from IMU: 0x%04X", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_INIT_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise IMU failure event 0x%04X",
                        EVT_IMU_INIT_FAILURE
                    );
                }

                /* Clear the I2C action */
                i2c_error = I2c_clear_device_action(&IMU_MAIN_I2C_DEVICE);
                if (i2c_error != ERROR_NONE) {
                    DEBUG_ERR(
                        "CRITICAL: could not clear the I2C device action"
                    );
                }

                return false;
            }

            /* Read successful, set the bypass enable pin high so that the
             * magnetometer, which is actually a separate chip inside the IMU, 
             * can be directly accessed by the component over I2C.
             * 
             * Note: The old code base ored the config pin with 0b0011_0111 
             * (55), which sets:
             *  - LATCH_INT_EN,
             *  - INT_ANYRD_2CLEAR,
             *  - FSYNC_INT_MODE_EN,
             *  - BYPASS_EN,
             *  - RESERVED
             * I beleive this is wrong, for one it's setting a reserved bit
             * which is very bad practice. Additionally interrupts are enabled,
             * which is never mentioned in the explanation of what the old code
             * is doing. I believe that this is a mistake, as 55 happens to be 
             * the decimal address of the INT_PIN_CFG register. */
            DEBUG_DBG("Old INT_PIN_CFG = 0x%02X", int_pin_cfg);
            int_pin_cfg |= IMU_INT_PIN_CFG_BYPASS_ENABLE_BIT;
            DEBUG_DBG("New INT_PIN_CFG = 0x%02X", int_pin_cfg);

            /* Send the changed config register back to the IMU */
            i2c_error = I2c_device_send_bytes(
                &IMU_MAIN_I2C_DEVICE, 
                &int_pin_cfg,
                1
            );
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while sending INT_CFG_PIN: 0x%04X", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_INIT_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise EVT_IMU_INIT_FAILURE"
                    );
                }

                return false;
            }

            /* Advance to next substate */
            DP.IMU.SUBSTATE = IMU_SUBSTATE_INIT_WAIT_INT_PIN_CFG_SEND_COMPLETE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_INIT_WAIT_INT_PIN_CFG_SEND_COMPLETE: ;

            /* Wait for the I2c operation to finish */
            if (!Imu_wait_i2c_action_finished(
                &IMU_MAIN_I2C_DEVICE,
                &i2c_action_finished,
                &i2c_action_success,
                EVT_IMU_INIT_FAILURE
            )) {
                /* Imu_wait_i2c_action_finished will set error code and 
                 * message */
                return false;
            }

            /* If the operation isn't finished return */
            if (!i2c_action_finished) {
                return true;
            }

            /* Otherwise check for action failure */
            if (!i2c_action_success) {
                /* The wait function will set the appropriate error messages */
                return false;
            }

            /* On success we clear the action, as this was a send there's no
             * bytes to read */
            i2c_error = I2c_clear_device_action(&IMU_MAIN_I2C_DEVICE);
            if (i2c_error != ERROR_NONE) {
                DEBUG_ERR(
                    "I2C error while sending INT_PIN_CFG to IMU: 0x%04X", 
                    i2c_error
                );
                DP.IMU.ERROR_CODE = IMU_ERROR_I2C_ERROR;
                DP.IMU.I2C_ERROR_CODE = i2c_error;

                /* Raise the failed event */
                if (!EventManager_raise_event(
                    EVT_IMU_INIT_FAILURE
                )) {
                    DEBUG_ERR(
                        "CRITICAL: could not raise IMU failure event 0x%04X",
                        EVT_IMU_INIT_FAILURE
                    );
                }

                /* Clear the I2C action */
                i2c_error = I2c_clear_device_action(&IMU_MAIN_I2C_DEVICE);
                if (i2c_error != ERROR_NONE) {
                    DEBUG_ERR(
                        "CRITICAL: could not clear the I2C device action"
                    );
                }

                return false;
            }

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_INIT_WAIT_MAGNE_MODE_RESET_COMPLETE: ;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_INIT_WAIT_MAGNE_MODE_SET_COMPLETE: ;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_INIT_WAIT_CFG_RECV_COMPLETE: ;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_INIT_WAIT_CFG_SEND_COMPLETE: ;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_INIT_WAIT_GYRO_CFG_RECV_COMPLETE: ;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case IMU_SUBSTATE_INIT_WAIT_GYRO_CFG_SEND_COMPLETE: ;


            /* Set the initialised flag in the DP */
            DP.IMU.INITIALISED = true;

            break;
        default:
            DEBUG_ERR(
                "Invalid DP.IMU.SUBSTATE value: %d",
                DP.IMU.SUBSTATE
            );
            DP.IMU.ERROR_CODE = IMU_ERROR_INVALID_SUBSTATE;
            return false;

    }
    #pragma GCC diagnostic pop

    return true;
}