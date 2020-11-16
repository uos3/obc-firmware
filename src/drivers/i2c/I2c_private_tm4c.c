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
     * 1. Wait for master to become non-busy
     *      - If the number of busy checks is greater than 3 error
     *      - Try to check master not busy for 5 times
     *      - If busy return out to step, waiting on not busy, increment number
     *        of busy checks
     * 2. Finished
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
                p_action_in->num_master_busy_checks 
                >= 
                I2C_MAX_NUM_MASTER_BUSY_MAJOR_CHECKS
            ) {
                DEBUG_ERR(
                   "I2C master module %d has been busy for %d major loops, now\
                    considered unresponsive.",
                    p_action_in->device.module,
                    p_action_in->num_master_busy_checks
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
                DEBUG_DBG(
                    "I2C master module %d still busy after minor check.",
                    p_action_in->device.module
                );
            }

            /* Increment step */
            p_action_in->step++;

            __attribute__ ((fallthrough));
        case 2:
            
            /* Set the action status as successful */
            p_action_in->status = I2C_ACTION_STATUS_SUCCESS;

            break;
        default:
            DEBUG_ERR("Reached unexpected step of single send action");
            return I2C_ERROR_UNEXPECTED_ACTION_STEP;

    }

    return I2C_ERROR_NONE;
}

bool I2c_devices_equal(I2c_Device *p_a_in, I2c_Device *p_b_in) {
    return (p_a_in->module == p_b_in->module) 
        && 
        (p_a_in->address == p_b_in->address);
}