/**
 * @ingroup gpio
 * 
 * @file Gpio_private_tm4c.c
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Private GPIO driver for the firmware.
 * 
 * Implements the GPIO driver for the TM4C.
 * 
 * @version 0.1
 * @date 2020-12-08
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard Lirary */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Internal */
#include "drivers/gpio/Gpio_public.h"
#include "drivers/gpio/Gpio_private.h"
#include "drivers/gpio/Gpio_errors.h"
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"

/* External */
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* Create an array of each GPIO module (with each port and each pin defined
 * within external includes)
 * 
 * Note from TivaWare manual: Ports ABCD each have 8 pins 0-7, but port E has
 * 6 pins 0-5, and port F has 5 pins 0-4.
 * 
 * TODO: Find out if there was a reason why 201920 group had all 8 for each
 * port. It is probably an error however there may be a reason behind it. */
Gpio_Module GPIO_MODULES[GPIO_MAX_NUM_GPIOS] = {
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, GPIO_MODE_INPUT, false},

    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, GPIO_MODE_INPUT, false},

    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, GPIO_MODE_INPUT, false},

    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, GPIO_MODE_INPUT, false},

    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_INPUT, false},

    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_INPUT, false},
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_INPUT, false},
} ;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Gpio_init(uint32_t *p_gpio_modules_in, size_t num_gpio_modules_in, Gpio_Mode mode_in) {
    /* Initialises the GPIO */

    /* Loop through modules being initialised */
    for (int i = 0; i < num_gpio_modules_in; ++i) {
        
        /* Get a reference to the GPIO */
        Gpio_Module *p_gpio_module = &GPIO_MODULES[p_gpio_modules_in[i]];

        /* If the GPIO has already been initialised, do not initialise it
         * again, and return a warning message (but not an error) */
        if (p_gpio_module->initialised) {
            DEBUG_WRN(
                "Gpio_init() called on GPIO module %d when already initialised",
                p_gpio_modules_in[i]
            );

            continue;
        }

        /* If the peripheral is not ready, reset and enable it */
        if (!SysCtlPeripheralReady(p_gpio_module->peripheral)) {
            SysCtlPeripheralReset(p_gpio_module->peripheral);
            SysCtlPeripheralEnable(p_gpio_module->peripheral);

            /* Attempt to initialise the GPIO peripheral, looping through
             * number of attempts i. */
            for (int i = 0; i < GPIO_MAX_NUM_PERIPHERAL_READY_CHECKS; ++i) {
                if (SysCtlPeripheralReady(p_gpio_module->peripheral)) {
                    /* If the peripheral is ready, break out of the loop, as
                     * it does not need to attempt again. */
                    break;
                }

                if (i = GPIO_MAX_NUM_PERIPHERAL_READY_CHECKS) {
                    /* If the number of attempts has reached the maximum,
                     * raise an error. */
                    DEBUG_ERR("Failed to enable GPIO %d peripheral", p_gpio_modules_in[i]);
                    /* TODO: Raise an event */
                    return GPIO_ERROR_PERIPHERAL_ENABLE_FAILED;
                }
            }
        }

        /* Initialise the mode of the GPIO */
        if (p_gpio_module->mode != mode_in) {
            /* Set the pin of the module to either input or output */
            switch(mode_in) {
                
                case GPIO_MODE_INPUT:
                    GPIOPinTypeGPIOInput(p_gpio_module->port, p_gpio_module->pin);
                    break;
                
                case GPIO_MODE_OUTPUT:
                    GPIOPinTypeGPIOOutput(p_gpio_module->port, p_gpio_module->pin);
                    break;
                
                default:
                    /* If the mode is neither GPIO_MODE_INPUT or
                     * GPIO_MODE_OUTPUT, return an error */
                    DEBUG_ERR("Unexpected mode for GPIO module %d", p_gpio_modules_in[i]);
                    return GPIO_ERROR_UNEXPECTED_MODE;
            }
            
            /* Set the mode of the GPIO */
            p_gpio_module->mode = mode_in;
        }

        /* If this point has reached without any errors, return ERROR_NONE */
        return ERROR_NONE;
    }
}