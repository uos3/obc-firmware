/**
 * @ingroup i2c
 * 
 * @file I2c_private_tm4c.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Private source for the I2C driver.
 * 
 * This file implements the I2C driver for TM4C targets.
 * 
 * @version 0.1
 * @date 2020-11-11
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
#include "system/event_manager/EventManager_public.h"
#include "drivers/i2c/I2c_private.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

I2c I2C;

I2c_Module I2C_MODULES[I2C_NUM_MODULES] = {
    /* I2C module 0 */
    {
        SYSCTL_PERIPH_GPIOB,
        SYSCTL_PERIPH_I2C0,
        GPIO_PORTB_BASE,
        I2C0_BASE,
        GPIO_PB2_I2C0SCL,
        GPIO_PB3_I2C0SDA,
        GPIO_PIN_2,
        GPIO_PIN_3,
        false,
        false
    },

    /* I2C module 1 */
    {
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_I2C1,
        GPIO_PORTA_BASE,
        I2C1_BASE,
        GPIO_PA6_I2C1SCL,
        GPIO_PA7_I2C1SDA,
        GPIO_PIN_6,
        GPIO_PIN_7,
        false,
        false
    },

    /* I2C module 2 */
    {
        SYSCTL_PERIPH_GPIOE,
        SYSCTL_PERIPH_I2C2,
        GPIO_PORTE_BASE,
        I2C2_BASE,
        GPIO_PE4_I2C2SCL,
        GPIO_PE5_I2C2SDA,
        GPIO_PIN_4,
        GPIO_PIN_5,
        false,
        false
    },

    /* I2C module 3 */
    {
        SYSCTL_PERIPH_GPIOD,
        SYSCTL_PERIPH_I2C3,
        GPIO_PORTD_BASE,
        I2C3_BASE,
        GPIO_PD0_I2C3SCL,
        GPIO_PD1_I2C3SDA,
        GPIO_PIN_0,
        GPIO_PIN_1,
        false,
        false
    }
};

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

I2c_ErrorCode I2c_action_single_send(I2c_ActionSingleSend *p_action_in) {
    /* No need to check for initialisation as this is done in the step 
     * function */
    
    /*
     * Steps required for this action:
     * 
     * 0. Setup:
     *      - Set the slave device address
     *      - Put the data onto the bus
     *      - Control for single command
     * 1. Wait for master to become non-busy:
     *      - If the number of busy checks is greater than 3 error
     *      - Try to check master not busy for 5 times
     *      - If busy return out to step, waiting on not busy, increment number
     *        of busy checks
     * 2. Finished:
     *      - Check no error occured
     *      - Mark as successful if not
     */

    /* Get a pointer to the I2C module this device is associated with */
    I2c_Module *p_i2c_module = &I2C_MODULES[p_action_in->device.module];

    /* Switch based on the current step index */
    switch (p_action_in->step) {
        case 0:
            /* Set the I2C module to use the device's slave address in send
             * mode (false) */
            I2CMasterSlaveAddrSet(
                p_i2c_module->base_i2c, 
                p_action_in->device.address, 
                false
            );

            /* Place the byte onto the bus */
            I2CMasterDataPut(p_i2c_module->base_i2c, p_action_in->byte);

            /* Instruct the module to perform the single send */
            I2CMasterControl(
                p_i2c_module->base_i2c, 
                I2C_MASTER_CMD_SINGLE_SEND
            );

            /* Increment the setp index */
            p_action_in->step++;
        
            /* 
             * This GCC attribute prevents the fallthrough warning for this
             * case. Usually fallthrough in switches is bad, but here we
             * actually want this so that we do not interrupt the function when
             * it has the possibility to actually make progress. Therefore this
             * attribute is used to keep the fallthrough warning enabled but to
             * say that here it is ok
             */
            __attribute__ ((fallthrough));
        case 1:
            /* Check that the number of checks is less than the limit */
            if (
                p_action_in->num_master_busy_major_checks 
                >= 
                I2C_MAX_NUM_MASTER_BUSY_MAJOR_CHECKS
            ) {
                DEBUG_ERR(
                   "I2C master module %d has been busy for %d major loops, now\
                    considered unresponsive.",
                    p_action_in->device.module,
                    p_action_in->num_master_busy_major_checks
                );
                p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                p_action_in->error = I2C_ERROR_MODULE_MASTER_BUSY;
                return I2C_ERROR_MODULE_MASTER_BUSY;
            }

            /* Check for master busy in the minor loop */
            bool master_busy = true;
            for (int i = 0; i < I2C_MAX_NUM_MASTER_BUSY_MINOR_CHECKS; ++i) {
                master_busy = I2CMasterBusy(p_i2c_module->base_i2c);

                /* If the master is already not busy exit before the end of the
                 * loop */
                if (!master_busy) {
                    break;
                }
            }

            /* If the master is busy exit and return to main loop control,
             * otherwise carry on to next step */
            if (master_busy) {
                /* Increment major check number */
                p_action_in->num_master_busy_major_checks++;

                DEBUG_DBG(
                    "I2C master module %d still busy after minor check.",
                    p_action_in->device.module
                );
                return I2C_ERROR_NONE;
            }

            /* Increment step */
            p_action_in->step++;

            __attribute__ ((fallthrough));
        case 2:
            
            /* Check for errors */
            p_action_in->error 
                = I2c_check_master_error(p_i2c_module->base_i2c);

            /* If no error return success, otherwise return failure */
            if (p_action_in->error == I2C_ERROR_NONE) {
                p_action_in->status = I2C_ACTION_STATUS_SUCCESS;
                return I2C_ERROR_NONE;
            }
            else {
                p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                return p_action_in->error;
            }
            
        default:
            DEBUG_ERR("Reached unexpected step of single send action");
            return I2C_ERROR_UNEXPECTED_ACTION_STEP;

    }

}

I2c_ErrorCode I2c_action_single_recv(I2c_ActionSingleRecv *p_action_in) {
    /* Steps required for this action:
     * 
     * Step 0: Setup:
     *     -Set the slave device address (Also, set it up to receive: See
     *         TivaWare Peripheral Driver Library pg.323 for more information)
     *     -Receive data from the device
     * Step 1: Wait for Master to Become Non-Busy:
     *     -Check the number of major and minor loop attempts
     *     -Increment number of checks by 1 if the master is still busy
     *     -Continue to the next step if the master is not busy
     * Step 2: Finish
     *     -Check for errors and set action status as successful if no errors
     *         occured
     */

    /* Get a pointer to the I2C module this device is associated with */
    I2c_Module *p_i2c_module = &I2C_MODULES[p_action_in->device.module];

    switch(p_action_in->step) {
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
                I2C_MASTER_CMD_SINGLE_RECEIVE
            );

            /* Receive byte from the device */
            /* TODO: Why is this done before the bus is confirmed not busy? */
            p_action_in->byte = (uint8_t)I2CMasterDataGet(p_i2c_module->base_i2c);

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

            /* Check for errors, setting the action status accordingly. Return
             * the error if there is one. */
            p_action_in->error = I2c_check_master_error(p_i2c_module->base_i2c);

            if (p_action_in->error == I2C_ERROR_NONE) {
                p_action_in->status = I2C_ACTION_STATUS_SUCCESS;
                return I2C_ERROR_NONE;
            }
            else {
                p_action_in->status = I2C_ACTION_STATUS_FAILURE;
                return p_action_in->error;
            }

            break;
        
        default:
            /* If the step is any value other than expected, raise an error */
            DEBUG_ERR("Reached unexpected step of single receive action");
            return I2C_ERROR_UNEXPECTED_ACTION_STEP;
    }

}

bool I2c_devices_equal(I2c_Device *p_a_in, I2c_Device *p_b_in) {
    return (p_a_in->module == p_b_in->module) 
        && 
        (p_a_in->address == p_b_in->address);
}

I2c_ErrorCode I2c_check_master_error(uint32_t i2c_base_addr_in) {
    /* Read error code from I2C module */
    uint32_t i2c_error = I2CMasterErr(i2c_base_addr_in);

    /* Return the translated error code */
    switch (i2c_error) {
        case I2C_MASTER_ERR_NONE:
            return I2C_ERROR_NONE;

        /* Address acknowledge failed */
        case I2C_MASTER_ERR_ADDR_ACK:
            DEBUG_ERR("I2C slave did not acknowledge address");
            return I2C_ERROR_ADDRESS_ACK_FAILED;

        /* Data acknowledge failed */
        case I2C_MASTER_ERR_DATA_ACK:
            DEBUG_ERR("I2C slave did not acknowledge data");
            return I2C_ERROR_DATA_ACK_FAILED;

        /* Arbitration lost */
        case I2C_MASTER_ERR_ARB_LOST:
            DEBUG_ERR("I2C arbitration lost");
            return I2C_ERROR_ARBITRATION_LOST;
        
        /* Unknown */
        default:
            return I2C_ERROR_UNKNOWN_TIVAWARE_ERROR;
    }
}

I2c_ErrorCode I2c_action_burst_send_master_busy_check(
    I2c_ActionBurstSend *p_action_in,
    bool *p_master_busy_out
) {
        
    /* Get a pointer to the I2C Module Associated with the Device. */
    I2c_Module *p_i2c_module = &I2C_MODULES[p_action_in->device.module];

    /* Check that the number of major checks is greater than or equal to the 
     * specified maximum, and return an error if it is. */
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

        p_action_in->status = I2C_ACTION_STATUS_FAILURE;
        p_action_in->error = I2C_ERROR_MODULE_MASTER_BUSY;

        /* Emmit the finished event */
        if (!EventManager_raise_event(EVT_I2C_ACTION_FINISHED)) {
            DEBUG_ERR("CRITICAL: Error raising event during error");
        }

        return p_action_in->error;
    }

    /* Check if the master is busy within the minor loop. */
    for (int i = 0; i < I2C_MAX_NUM_MASTER_BUSY_MINOR_CHECKS; ++i) {

        /* Sets the value of the master_busy output bool. */
        *p_master_busy_out = I2CMasterBusy(p_i2c_module->base_i2c);

        /* If the master is not busy, exit the loop, resetting the major
         * checks to 0 for next time. */
        if (!*p_master_busy_out) {
            p_action_in->num_master_busy_major_checks = 0;
            return I2C_ERROR_NONE;
        }
    }

    /* If the master is still busy at the end of the minor loop, increment the
     * number of major checks. */
    p_action_in->num_master_busy_major_checks++;
    return I2C_ERROR_NONE;
}

I2c_ErrorCode I2c_lock_module(I2c_Device *p_device_in) {
    /* No need to check if I2C initialised because the calling function shall
     * allways do this check first */

    /* Check if the module is already locked */
    if (I2C.module_locked[p_device_in->module]) {
        DEBUG_ERR(
            "Device (%d, %d) tried to lock the module but another device already has the lock.",
            p_device_in->module,
            p_device_in->address
        );
        return I2C_ERROR_MODULE_LOCKED_BY_ANOTHER_DEVICE;
    }
    /* Otherwise raise the lock */
    else {
        DEBUG_TRC("Module %d locked", p_device_in->module);
        I2C.module_locked[p_device_in->module] = true;
        return I2C_ERROR_NONE;
    }
}

I2c_ErrorCode I2c_action_burst_recv_master_busy_check(
    I2c_ActionBurstRecv *p_action_in,
    bool *p_master_busy_out
) {
        
    /* Get a pointer to the I2C Module Associated with the Device. */
    I2c_Module *p_i2c_module = &I2C_MODULES[p_action_in->device.module];

    /* Check that the number of major checks is greater than or equal to the 
     * specified maximum, and return an error if it is. */
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

        p_action_in->status = I2C_ACTION_STATUS_FAILURE;
        p_action_in->error = I2C_ERROR_MODULE_MASTER_BUSY;

        /* Emmit the finished event */
        if (!EventManager_raise_event(EVT_I2C_ACTION_FINISHED)) {
            DEBUG_ERR("CRITICAL: Error raising event during error");
        }

        return p_action_in->error;
    }

    /* Check if the master is busy within the minor loop. */
    for (int i = 0; i < I2C_MAX_NUM_MASTER_BUSY_MINOR_CHECKS; ++i) {

        /* Sets the value of the master_busy output bool. */
        *p_master_busy_out = I2CMasterBusy(p_i2c_module->base_i2c);

        /* If the master is not busy, exit the loop, resetting the major
         * checks to 0 for next time. */
        if (!*p_master_busy_out) {
            p_action_in->num_master_busy_major_checks = 0;
            return I2C_ERROR_NONE;
        }
    }

    /* If the master is still busy at the end of the minor loop, increment the
     * number of major checks. */
    p_action_in->num_master_busy_major_checks++;
    return I2C_ERROR_NONE;
}