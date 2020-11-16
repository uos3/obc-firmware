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

I2c_ErrorCode I2c_action_burst_send(I2c_ActionBurstSend *p_action_in) {
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
    bool exit_action = false;
    I2c_ErrorCode master_busy = I2C_ERROR_NONE;
    I2c_ErrorCode master_error = I2C_ERROR_NONE;

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

            /* Check for busy */
            exit_action = false;
            master_busy = I2c_burst_send_wait_master_not_busy(
                p_action_in,
                p_i2c_module,
                &exit_action
            );

            /* If the busy funcion needs to exit do */
            if (exit_action) {
                return master_busy;
            }

            /* Increment step */
            p_action_in->step++;

            __attribute__ ((fallthrough));
        /* Check for send error */
        case 2:

            /* Check for an error at this point */
            master_error = I2c_check_master_error(
                p_i2c_module->base_i2c
            );

            /* If there is an error set the action as failed and return the
             * error */
            if (master_error != I2C_ERROR_NONE) {
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

                        /* Check for busy */
                        exit_action = false;
                        master_busy 
                            = I2c_burst_send_wait_master_not_busy(
                                p_action_in,
                                p_i2c_module,
                                &exit_action
                            );

                        /* If the busy funcion needs to exit do */
                        if (exit_action) {
                            return master_busy;
                        }

                        /* Increment substep */
                        p_action_in->substep++;

                        __attribute__ ((fallthrough));
                    case 2:
                        /* Check for an error at this point */
                        master_error = I2c_check_master_error(
                            p_i2c_module->base_i2c
                        );

                        /* If there is an error save it and return failure */
                        if (master_error != I2C_ERROR_NONE) {
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

            /* Check for busy */
            exit_action = false;
            master_busy = I2c_burst_send_wait_master_not_busy(
                p_action_in,
                p_i2c_module,
                &exit_action
            );

            /* If the busy funcion needs to exit do */
            if (exit_action) {
                return master_busy;
            }

            /* Increment step */
            p_action_in->step++;

            __attribute__ ((fallthrough));
        /* Check for send error */
        case 6:

            /* Check for an error at this point */
            master_error = I2c_check_master_error(
                p_i2c_module->base_i2c
            );

            /* If there is an error set the action as failed and return the
             * error */
            if (master_error != I2C_ERROR_NONE) {
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
            return I2C_ERROR_NONE;
        
        default:
            DEBUG_ERR("Unrecognised I2C action step");
            return I2C_ERROR_UNEXPECTED_ACTION_STEP;
    }
}

I2c_ErrorCode I2c_action_burst_recv(I2c_ActionBurstRecv *p_action_in) {

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

I2c_ErrorCode I2c_burst_send_wait_master_not_busy(
    I2c_ActionBurstSend *p_action_in,
    I2c_Module *p_i2c_module_in,
    bool *p_exit_action_out
) {

    /* By default the action should not exit */
    *p_exit_action_out = false;

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
        *p_exit_action_out = true;
        return I2C_ERROR_MODULE_MASTER_BUSY;
    }

    /* Check for master busy in the minor loop */
    bool master_busy = true;
    for (int i = 0; i < I2C_MAX_NUM_MASTER_BUSY_MINOR_CHECKS; ++i) {
        master_busy = I2CMasterBusy(p_i2c_module_in->base_i2c);

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

        /* Set that the action should be exited */
        *p_exit_action_out = true;

        DEBUG_DBG(
            "I2C master module %d still busy after minor check.",
            p_action_in->device.module
        );
        return I2C_ERROR_NONE;
    }

    /* Zero the major check counter */
    p_action_in->num_master_busy_major_checks = 0;

    return I2C_ERROR_NONE;
}