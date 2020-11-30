/**
 * @ingroup i2c
 * 
 * @file I2c_action_burst_send_tm4c.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Private burst send action function for the I2C Driver
 * 
 * This file adds the action for burst receive for the I2C. It was created
 * as a new file to avoid functions over 100 lines long, in line with the
 * software writing standards.
 * 
 * @version 0.1
 * @date 2020-11-20
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* External includes */
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"

#include "inc/hw_memmap.h"

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/i2c/I2c_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode I2c_action_burst_send(I2c_ActionBurstSend *p_action_in) {
    /*
     * Steps for this action:
     * 
     * 0. Setup:
     *      - Set the slave device address
     *      - Put the data onto the bus
     *      - Control for burst send start
     * 1. Wait on master not busy:
     *      - Check number of major attempts less than max
     *      - Try minor attempts
     *      - If master not busy continue to next step
     *      - If master busy increment number of major checks and exit
     * 2. Check for errors:
     *      - If error exit
     * 3. Burst send:
     *      - For all bytes between bytes_sent and length - 1:
     *          a:
     *              - Put data on bus
     *              - Control for burst send continue
     *          b:         
     *              - Wait for master to not be busy (see step 1)
     *          c:
     *              - Check for errors:
     *                  - If error control for burst send error stop
     *                  - Exit
     * 4. Final byte send:
     *      - Send final byte
     *      - Control for burst send finish
     * 5. Wait on master not busy (see step 1)
     * 6. Check for errors:
     *      - If error control for burst send error stop
     *      - Exit
     */

    /* Get a pointer to the I2C module this device is associated with */
    I2c_Module *p_i2c_module = &I2C_MODULES[p_action_in->device.module];

    /* Values declared in the switch statement */
    bool master_busy = true;
    ErrorCode master_error = ERROR_NONE;

    switch (p_action_in->step) {
        /* Setup and send first byte */
        case 0:
            /* Set the slave address in send mode (false) */
            I2CMasterSlaveAddrSet(
                p_i2c_module->base_i2c, 
                p_action_in->device.address,
                false
            );

            /* Put the first byte on the bus */
            I2CMasterDataPut(p_i2c_module->base_i2c, p_action_in->p_bytes[0]);

            /* Start the send operation */
            I2CMasterControl(
                p_i2c_module->base_i2c, 
                I2C_MASTER_CMD_BURST_SEND_START
            );

            /* Increment the step */
            p_action_in->step++;

            __attribute__ ((fallthrough));
        /* Wait for not busy */
        case 1:

            /* Check if the I2C is busy */
            p_action_in->error = I2c_action_burst_send_master_busy_check(
                p_action_in,
                &master_busy
            );
            
            /* If the master is busy, set the action as failure, show a debug
             * message, and return the error. */
            if (master_busy) {
                return p_action_in->error;
            }

            /* If the master is not busy, increment the step and continue. */
            else {
                p_action_in->step++;
            }

            __attribute__ ((fallthrough));
        /* Check for send error */
        case 2:

            /* Check for an error at this point */
            master_error = I2c_check_master_error(
                p_i2c_module->base_i2c
            );

            /* If there is an error set the action as failed and return the
             * error */
            if (master_error != ERROR_NONE) {
                /* Control for failure, which allows the I2C bus to gracefully
                 * recover */
                I2CMasterControl(
                    p_i2c_module->base_i2c,
                    I2C_MASTER_CMD_BURST_SEND_ERROR_STOP
                );

                DEBUG_ERR("Error sending first byte to device.");
                p_action_in->error = master_error;
                p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                return master_error;
            }

            /* 
             * A byte has been sent now so increment the number of sent bytes
             */
            p_action_in->num_bytes_sent++;

            /* Initialise substep value for the next step */
            p_action_in->substep = 0;

            /* Increment step */
            p_action_in->step++;

            __attribute__ ((fallthrough));
        /* Send middle bytes */
        case 3:

            /* For all bytes that haven't been sent yet (except the last byte
             * since that one has a special condition) */
            for (
                size_t i = p_action_in->num_bytes_sent; 
                i < p_action_in->length - 1; 
                ++i
            ) {
                /* Substate machine */
                switch (p_action_in->substep) {
                    case 0:
                        /* Put data on bus */
                        I2CMasterDataPut(
                            p_i2c_module->base_i2c, 
                            p_action_in->p_bytes[i]
                        );

                        /* Set control for burst send continue */
                        I2CMasterControl(
                            p_i2c_module->base_i2c,
                            I2C_MASTER_CMD_BURST_SEND_CONT
                        );

                        /* Increment the substep */
                        p_action_in->substep++;

                        __attribute__ ((fallthrough));
                    case 1:

                        /* Check if the I2C is busy */
                        master_busy = true;
                        p_action_in->error = I2c_action_burst_send_master_busy_check(
                            p_action_in,
                            &master_busy
                        );
                        
                        /* If the master is busy, set the action as failure, show a debug
                        * message, and return the error. */
                        if (master_busy) {
                            return p_action_in->error;
                        }

                        /* If the master is not busy, increment the step and continue. */
                        else {
                            p_action_in->step++;
                        }

                        __attribute__ ((fallthrough));
                    case 2:
                        /* Check for an error at this point */
                        master_error = I2c_check_master_error(
                            p_i2c_module->base_i2c
                        );

                        /* If there is an error save it and return failure */
                        if (master_error != ERROR_NONE) {
                            /* Control for failure, which allows the I2C bus to
                             * gracefully recover */
                            I2CMasterControl(
                                p_i2c_module->base_i2c,
                                I2C_MASTER_CMD_BURST_SEND_ERROR_STOP
                            );

                            DEBUG_ERR("Error sending byte %d to device.", i);
                            p_action_in->error = master_error;
                            p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                            return master_error;
                        }

                        /* 
                         * A byte has been sent now so increment the number of 
                         * sent bytes
                         */
                        p_action_in->num_bytes_sent++;

                        /* Initialise substep value for the next step */
                        p_action_in->substep = 0;
                        break;
                    default:
                        DEBUG_ERR("Unrecognised I2C burst send substep");
                        return I2C_ERROR_UNEXPECTED_ACTION_STEP;
                }
            }

            /* Increment the step */
            p_action_in->step++;

            __attribute__ ((fallthrough));
        /* Send final byte */
        case 4:

            /* Put data on bus */
            I2CMasterDataPut(
                p_i2c_module->base_i2c, 
                p_action_in->p_bytes[p_action_in->length - 1]
            );

            /* Set control for burst send continue */
            I2CMasterControl(
                p_i2c_module->base_i2c,
                I2C_MASTER_CMD_BURST_SEND_FINISH
            );

            /* Increment the step */
            p_action_in->step++;

            __attribute__ ((fallthrough));
        /* Wait for not busy */
        case 5:

            /* Check if the I2C is busy */
            master_busy = true;

            p_action_in->error = I2c_action_burst_send_master_busy_check(
                p_action_in,
                &master_busy
            );
            
            /* If the master is busy, set the action as failure, show a debug
             * message, and return the error. */
            if (master_busy) {
                return p_action_in->error;
            }

            /* If the master is not busy, increment the step and continue. */
            else {
                p_action_in->step++;
            }

            __attribute__ ((fallthrough));
        /* Check for send error */
        case 6:

            /* Check for an error at this point */
            master_error = I2c_check_master_error(
                p_i2c_module->base_i2c
            );

            /* If there is an error set the action as failed and return the
             * error */
            if (master_error != ERROR_NONE) {
                /* Control for failure, which allows the I2C bus to gracefully
                 * recover */
                I2CMasterControl(
                    p_i2c_module->base_i2c,
                    I2C_MASTER_CMD_BURST_SEND_ERROR_STOP
                );

                DEBUG_ERR("Error sending last byte to device.");
                p_action_in->error = master_error;
                p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                return master_error;
            }

            /* 
             * A byte has been sent now so increment the number of sent bytes
             */
            p_action_in->num_bytes_sent++;

            /* Set the action as successful */
            p_action_in->status = I2C_ACTION_STATUS_SUCCESS;

            /* Return since finished */
            return ERROR_NONE;
        
        default:
            DEBUG_ERR("Unrecognised I2C action step");
            return I2C_ERROR_UNEXPECTED_ACTION_STEP;
    }
}