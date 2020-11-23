/**
 * @ingroup i2c
 * 
 * @file I2c_action_burst_recv_tm4c.c
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Private burst receive action function for the I2C Driver
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

I2c_ErrorCode I2c_action_burst_recv(I2c_ActionBurstRecv *p_action_in) {
    /*
     * Steps required for this action:
     * 
     * Step 0: Setup:
     *     -Set the slave device address (Also, set it up to receive: See
     *         TivaWare Peripheral Driver Library pg.323 for more information)
     *     -Receive data from the device
     * Step 1: Wait for Master to become Non-Busy:
     *     -Check the number of major and minor loop attempts
     *     -Increment number of checks by 1 if the master is still busy
     *     -Continue to the next step if the master is not busy
     * Step 2: Check for Errors
     * 
     * Step 3: Loop/Burst Receive for all Bytes (CONTINUE):
     * 
     * Step 4: Finish/Burst Receive for last Byte (FINISHED):
     * 
     * Step 5: Wait for Master to Become Non-Busy:
     * 
     * Step 6: Finish
     * 
     */

    /* Get a pointer to the I2C Module this device is associated with */
    I2c_Module *p_i2c_module = &I2C_MODULES[p_action_in->device.module];
    I2c_ErrorCode master_error = I2C_ERROR_NONE;

    switch (p_action_in->step) {
        case 0:
            /* Set address as a send and use dataput onto register, must
             * send onto register to prepare the I2C to receive.
             * (false = send in this case). */
            I2CMasterSlaveAddrSet(
                p_i2c_module->base_i2c,
                p_action_in->device.address,
                false
            );

            I2CMasterDataPut(
                p_i2c_module->base_i2c,
                p_action_in->reg
            );

            I2CMasterControl(
                p_i2c_module->base_i2c,
                I2C_MASTER_CMD_SINGLE_SEND
            );
            
            /* Set the I2C to receive bytes from the device by
             * setting the slave address (true = receive in this case) */
            I2CMasterSlaveAddrSet(
                p_i2c_module->base_i2c,
                p_action_in->device.address,
                true
            );

            /* Tell the bus to receive (single) */
            I2CMasterControl(
                p_i2c_module->base_i2c,
                I2C_MASTER_CMD_BURST_RECEIVE_START
            );

            /* Receive byte from the device */
            /* TODO: Why is busy check done before the bus is confirmed not
             * busy?
             * FIXME send and recv, single and burst: double check: is an extra
             * busy check required, or is it enough to just swap
             *  case 0 and 1? */
            p_action_in->p_bytes[0] = (uint8_t)I2CMasterDataGet(p_i2c_module->base_i2c);

            /* Increase the step by 1 */
            p_action_in->step++;

            __attribute__ ((fallthrough));
        
        case 1:
            /* Check that the number of major checks is below the set maximum,
             * and return an error if the maximum has been reached */
            if (
                p_action_in->num_master_busy_major_checks
                >=
                I2C_MAX_NUM_MASTER_BUSY_MAJOR_CHECKS
            ) {
                DEBUG_ERR(
                    "I2C master module %d has been busy for %d major loops and is now marked as unresponsive.",
                    p_action_in->device.module,
                    p_action_in->num_master_busy_major_checks
                );

                /* Set the action status as a failure if the maximum number
                 * of checks has been reached. */
                p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                p_action_in->error = I2C_ERROR_MODULE_MASTER_BUSY;

                return p_action_in->error;
            }

            /* Check if the I2C is busy within the minor loops */
            /* TODO: Should this be declared at the start of the function,
             * as it is also used later on? */
            bool master_busy = true;
            for (int i = 0; i < I2C_MAX_NUM_MASTER_BUSY_MINOR_CHECKS; ++i) {
                master_busy = I2CMasterBusy(p_i2c_module->base_i2c);

                /* If the master is not busy at any point in the loop, exit
                 * the loop, as there is only an issue if it has been busy
                 * for the maximum number of checks */
                if (!master_busy) {
                    break;
                }
            }

            /* If the master is busy for all minor checks, exit to the major
             * loop and increase step by 1. If the master is not busy,
             * continue to the next step. */
            if (master_busy) {
                p_action_in->num_master_busy_major_checks++;

                /* If master is busy, exit and return to the main loop,
                 * assuming the number of checks has not reached the maximum */
                DEBUG_DBG(
                    "I2C master module %d still busy after minor check.",
                    p_action_in->device.module
                );
                return I2C_ERROR_NONE;
            }

            p_action_in->step++;

            __attribute__ ((fallthrough));

        case 2:
            
            /* Get the Error status */
            master_error = I2c_check_master_error(p_i2c_module->base_i2c);

            /* If the error status is anaything other than NONE, return
             * the error. If the error is NONE, continue. */
            if (master_error != I2C_ERROR_NONE) {
                I2CMasterControl(p_i2c_module->base_i2c,
                I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP
                );

                DEBUG_ERR("Error Receiving First Byte from Device");
                p_action_in->error = master_error;
                p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                return p_action_in->error;
            }

            for (
                size_t i = 1;
                i < p_action_in->length - 1;
                ++i
            ) {
                switch (p_action_in->substep) {
                    case 0:
                        I2CMasterControl(p_i2c_module->base_i2c, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
                        p_action_in->p_bytes[i] = (uint8_t)I2CMasterDataGet(p_i2c_module->base_i2c);
                        p_action_in->substep++;

                        __attribute__ ((fallthrough));

                    case 1:
                        if (
                            p_action_in->num_master_busy_major_checks
                            <=
                            I2C_MAX_NUM_MASTER_BUSY_MAJOR_CHECKS
                        ) {
                            DEBUG_ERR(
                                "I2C master module %d has been busy for %d major loops and is now marked as unresponsive.",
                                p_action_in->device.module,
                                p_action_in->num_master_busy_major_checks
                            );

                            /* Set the action status as a failure if the maximum number
                             * of checks has been reached. */
                            p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                            p_action_in->error = I2C_ERROR_MODULE_MASTER_BUSY;

                            return p_action_in->error;
                        }

                        /* Check if the I2C is busy within the minor loops */
                        bool master_busy = true;
                        for (int i = 0; i < I2C_MAX_NUM_MASTER_BUSY_MINOR_CHECKS; ++i) {
                            master_busy = I2CMasterBusy(p_i2c_module->base_i2c);

                            /* If the master is not busy at any point in the loop, exit
                            * the loop, as there is only an issue if it has been busy
                            * for the maximum number of checks */
                            if (!master_busy) {
                                break;
                            }
                        }

                        /* If the master is busy for all minor checks, exit to the major
                        * loop and increase step by 1. If the master is not busy,
                        * continue to the next step. */
                        if (master_busy) {
                            p_action_in->num_master_busy_major_checks++;
                        }

                        p_action_in->substep++;

                        __attribute__ ((fallthrough));
                    
                    case 2:

                        master_error = I2c_check_master_error(p_i2c_module->base_i2c);

                        if (master_error != I2C_ERROR_NONE) {
                            I2CMasterControl(p_i2c_module->base_i2c, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);

                            DEBUG_ERR("Error receiving byte %d from device", i);
                            p_action_in->error = master_error;
                            return master_error;
                        }

                        p_action_in->substep = 0;
                        break;
                    
                    default:
                        DEBUG_ERR("Unexpected I2C burst receive substep");
                        return I2C_ERROR_UNEXPECTED_ACTION_SUBSTEP;
                }
            }

            p_action_in->step++;

            __attribute__ ((fallthrough));
        
        case 3:
            I2CMasterControl(p_i2c_module->base_i2c, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
            p_action_in->p_bytes[p_action_in->length - 1] = (uint8_t)I2CMasterDataGet(p_i2c_module->base_i2c);

            p_action_in->step++;

            __attribute__ ((fallthrough));
        
        case 4:
            /* Check that the number of major checks is below the set maximum,
             * and return an error if the maximum has been reached */
            if (
                p_action_in->num_master_busy_major_checks
                >=
                I2C_MAX_NUM_MASTER_BUSY_MAJOR_CHECKS
            ) {
                DEBUG_ERR(
                    "I2C master module %d has been busy for %d major loops and is now marked as unresponsive.",
                    p_action_in->device.module,
                    p_action_in->num_master_busy_major_checks
                );

                /* Set the action status as a failure if the maximum number
                 * of checks has been reached. */
                p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                p_action_in->error = I2C_ERROR_MODULE_MASTER_BUSY;

                return p_action_in->error;
            }

            /* Check if the I2C is busy within the minor loops */
            master_busy = true;
            for (int i = 0; i < I2C_MAX_NUM_MASTER_BUSY_MINOR_CHECKS; ++i) {
                master_busy = I2CMasterBusy(p_i2c_module->base_i2c);

                /* If the master is not busy at any point in the loop, exit
                 * the loop, as there is only an issue if it has been busy
                 * for the maximum number of checks */
                if (!master_busy) {
                    break;
                }
            }

            /* If the master is busy for all minor checks, exit to the major
             * loop and increase step by 1. If the master is not busy,
             * continue to the next step. */
            if (master_busy) {
                p_action_in->num_master_busy_major_checks++;

                /* If master is busy, exit and return to the main loop,
                 * assuming the number of checks has not reached the maximum */
                DEBUG_DBG(
                    "I2C master module %d still busy after minor check.",
                    p_action_in->device.module
                );
                return I2C_ERROR_NONE;
            }

            p_action_in->step++;

            __attribute__ ((fallthrough));
        
        case 5:
            /* Get the Error status */
            master_error = I2c_check_master_error(p_i2c_module->base_i2c);

            /* If the error status is anaything other than NONE, return
             * the error. If the error is NONE, continue. */
            if (master_error != I2C_ERROR_NONE) {
                I2CMasterControl(p_i2c_module->base_i2c,
                I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP
                );

                DEBUG_ERR("Error Receiving First Byte from Device");
                p_action_in->error = master_error;
                p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                return p_action_in->error;
            }

            break;
        
        default:
            DEBUG_ERR("Unexpected I2C action step");
            return I2C_ERROR_UNEXPECTED_ACTION_STEP;
    }

    return I2C_ERROR_NONE;
}