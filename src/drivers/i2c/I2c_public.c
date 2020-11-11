/**
 * @ingroup i2c
 * 
 * @file I2c_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief I2C driver for the firmware.
 * 
 * See I2c_public.h for more information.
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* External includes */
#ifdef TARGET_TM4C
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"

#include "inc/hw_memmap.h"
#endif

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/i2c/I2c_public.h"
#include "drivers/i2c/I2c_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

I2c_ErrorCode I2c_init(uint32_t *p_modules_in, size_t num_modules_in) {

    /* Loop through each module */
    for (size_t i = 0; i < num_modules_in; ++i) {

        /* Get a reference to the module to init */
        I2c_Module *p_module = &I2C_MODULES[p_modules_in[i]];

        /* If the module is already initialised issue warning but don't do
         * anything 
         */
        if (p_module->initialised) {
            DEBUG_WRN(
                "I2c_init() called on module %d when already initialised.", 
                p_modules_in[i]
            );

            /* Go to the next module in the list */
            continue;
        }
        
        /* If the peripheral is not ready reset and enable it
         * TODO: The reset here in the old code reset the GPIO, this should
         * probably be I2C instead as GPIO is handled later, so just check this
         * is correct.
         */
        if (!SysCtlPeripheralReady(p_module->peripheral_i2c)) {
            SysCtlPeripheralReset(p_module->peripheral_i2c);
            SysCtlPeripheralEnable(p_module->peripheral_i2c);

            /* Attempt to initialise the I2C peripheral */
            bool enabled = false; 
            for (
                int attempt = 0;
                attempt < I2C_MAX_NUM_I2C_PERIPH_READY_CHECKS; 
                ++attempt
            ) {
                
                if (SysCtlPeripheralReady(p_module->peripheral_i2c)) {
                    enabled = true;
                    break;
                }
            }

            /* If the peripheral wasn't enabled before the max attempts was
             * reached return an error 
             */
            if (!enabled) {
                /* TODO: Mark the module as disabled? */
                DEBUG_ERR("Could not enable I2C module %d", p_modules_in[i]);
                return I2C_ERROR_I2C_PERIPH_ENABLE_FAILED;
            }
        }

        /* If the peripheral is not ready reset and enable it */
        if (!SysCtlPeripheralReady(p_module->peripheral_gpio)) {
            SysCtlPeripheralReset(p_module->peripheral_gpio);
            SysCtlPeripheralEnable(p_module->peripheral_gpio);

            /* Attempt to initialise the GPIO peripheral */
            bool enabled = false; 
            for (
                int attempt = 0;
                attempt < I2C_MAX_NUM_GPIO_PERIPH_READY_CHECKS; 
                ++attempt
            ) {
                
                if (SysCtlPeripheralReady(p_module->peripheral_gpio)) {
                    enabled = true;
                    break;
                }
            }

            /* If the peripheral wasn't enabled before the max attempts was
             * reached return an error 
             */
            if (!enabled) {
                /* TODO: Mark the module as disabled? */
                DEBUG_ERR("Could not enable GPIO module %d", p_modules_in[i]);
                return I2C_ERROR_GPIO_PERIPH_ENABLE_FAILED;
            }
        }

        /* Configure the GPIO pins for their I2C functions */
        GPIOPinConfigure(p_module->pin_scl_function);
        GPIOPinConfigure(p_module->pin_sda_function);

        /* Assign pins to the I2C peripheral */
        GPIOPinTypeI2CSCL(p_module->base_gpio, p_module->pin_scl);
        GPIOPinTypeI2C(p_module->base_gpio, p_module->pin_sda);

        /* Set the I2C peripheral clock rate */
        I2CMasterInitExpClk(
            p_module->base_i2c, 
            SysCtlClockGet(), 
            p_module->full_speed
        );

        /* Enable the master mode */
        I2CMasterEnable(p_module->base_i2c);

        /* Note: In the previous code a FIFO flush was done here, however the
         * TM4C123s have no FIFOs on the I2C, so this was removed. 
         */

        /* Set the module to be initialised */
        p_module->initialised = true;
    }

    /* Return success */
    return I2C_ERROR_NONE;
}