/**
 * @ingroup gpio
 * 
 * @file Gpio_public_tm4c.c
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public GPIO driver for the firmware.
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
#include "components/led/Led_public.h"

/* External */
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Gpio_init(GPIO_PIN_INDEX *p_gpio_pins_in, size_t num_gpio_pins_in, Gpio_Mode mode_in) {
    /* Initialises the GPIO */

    /* Loop through modules being initialised */
    for (int i = 0; i < num_gpio_pins_in; ++i) {
        
        /* Get a reference to the GPIO */
        Gpio_Module *p_gpio_pin = &GPIO_PINS[p_gpio_pins_in[i]];

        /* If the GPIO has already been initialised, do not initialise it
         * again, and return a warning message (but not an error) */
        if (p_gpio_pin->initialised == true) {
            DEBUG_INF("Gpio_init() called on GPIO module when already initialised");
            ++i;
        }

        /* If the peripheral is not ready, reset and enable it */
        if (!SysCtlPeripheralReady(p_gpio_pin->peripheral)) {
            SysCtlPeripheralReset(p_gpio_pin->peripheral);
            SysCtlPeripheralEnable(p_gpio_pin->peripheral);

            /* Attempt to initialise the GPIO peripheral, looping through
             * number of attempts i. */
            for (int i = 0; i < GPIO_MAX_NUM_PERIPHERAL_READY_CHECKS; ++i) {
                if (SysCtlPeripheralReady(p_gpio_pin->peripheral)) {
                    /* If the peripheral is ready, break out of the loop, as
                     * it does not need to attempt again. */
                    break;
                }

                if (i >= GPIO_MAX_NUM_PERIPHERAL_READY_CHECKS) {
                    /* If the number of attempts has reached the maximum,
                     * raise an error. */
                    DEBUG_ERR("Failed to enable GPIO peripheral");
                    return GPIO_ERROR_PERIPHERAL_ENABLE_FAILED;
                }
            }
        }

        /* Initialise the mode of the GPIO */
        if (p_gpio_pin->mode != mode_in) {
            /* Set the pin of the module to either input or output */
            switch(mode_in) {
                
                case GPIO_MODE_INPUT:
                    /* Configure the pin for use as GPIO input */
                    GPIOPinTypeGPIOInput(p_gpio_pin->port, p_gpio_pin->pin);
                    GPIOPadConfigSet(p_gpio_pin->port, p_gpio_pin->pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
                    DEBUG_INF("GPIO Input pin initialised");
                    break;
                
                case GPIO_MODE_OUTPUT:
                    /* Configure the pin for use as GPIO output */
                    GPIOPinTypeGPIOOutput(p_gpio_pin->port, p_gpio_pin->pin);
                    DEBUG_INF("GPIO Output pin initialised");
                    break;
                case GPIO_MODE_UART:
                    GPIOPinTypeUART(p_gpio_pin->port, p_gpio_pin->pin);
                    DEBUG_INF("GPIO UART pin initialised");
                    break;
                case GPIO_MODE_NONE:
                    DEBUG_INF("Attempted to initialise a GPIO pin with mode\
                        GPIO_PIN_NONE");
                    return GPIO_ERROR_NOT_INITIALISED;
                
                default:
                    /* If the mode is neither GPIO_MODE_INPUT or
                     * GPIO_MODE_OUTPUT, return an error */
                    DEBUG_ERR("Unexpected mode for GPIO module");
                    return GPIO_ERROR_UNEXPECTED_MODE;
            }
            
            /* Set the mode of the GPIO */
            p_gpio_pin->mode = mode_in;
        }

        /* Set the GPIO module as initialised */
        p_gpio_pin->initialised = true;

    }

    /* Set the GPIO driver state as initialised */
    /* GPIO.initialised = true; */
    /* If this point has reached without any errors, return ERROR_NONE */
    return ERROR_NONE;
}

ErrorCode Gpio_write(GPIO_PIN_INDEX gpio_id_number, uint8_t gpio_state_in) {
    Gpio_Module *p_gpio_pin = &GPIO_PINS[gpio_id_number];

    /* Check the GPIO has been initialised */
    if (!p_gpio_pin->initialised) {
        DEBUG_ERR("Attempted to write GPIO pin when the GPIO has not been initalised");
        return GPIO_ERROR_NOT_INITIALISED;
    }

    /* Check the GPIO to be read is not exceeding the number of GPIOs */
    if (gpio_id_number >= GPIO_NUM_GPIOS) {
        DEBUG_ERR("Gpio ID number has exceeded total number of GPIOs");
        return GPIO_ERROR_EXCEEDED_NUM_GPIOS;
    }

    switch (gpio_state_in) {
        case 1:
            GPIOPinWrite(p_gpio_pin->port, p_gpio_pin->pin, p_gpio_pin->pin);
            break;
        case 0:
            GPIOPinWrite(p_gpio_pin->port, p_gpio_pin->pin, gpio_state_in);
            break;
        default:
            DEBUG_ERR("Incorrect gpio_state_in");
            return GPIO_ERROR_INCORRECT_STATE_IN;
    }

    return ERROR_NONE;
}

ErrorCode Gpio_read(GPIO_PIN_INDEX gpio_id_number, uint8_t *p_gpio_value_out) {
    Gpio_Module *p_gpio_pin = &GPIO_PINS[gpio_id_number];

    /* Check the GPIO has been initialised */
    if (!p_gpio_pin->initialised) {
        DEBUG_ERR("Attempted to write GPIO pin when the GPIO has not been initalised");
        return GPIO_ERROR_NOT_INITIALISED;
    }

    /* Check the GPIO to be read is not exceeding the number of GPIOs */
    if (gpio_id_number >= GPIO_NUM_GPIOS) {
        DEBUG_ERR("Gpio ID number has exceeded total number of GPIOs");
        return GPIO_ERROR_EXCEEDED_NUM_GPIOS;
    }

    *p_gpio_value_out = (uint8_t)GPIOPinRead(p_gpio_pin->port, p_gpio_pin->pin);

    return ERROR_NONE;
}

void Gpio_port_a_int_handler(void) {
    uint32_t status;

    /* Get the interrupt status of the GPIO */
    status = GPIOIntStatus(GPIO_PORTA_BASE, true);

    if (status) {
        /* Clear the Port A interrupt Status */
        GPIOIntClear(GPIO_PORTA_BASE, status);

        /* Loop through all the pins in port A */
        Gpio_handle_interrupt(status, 0, 8);
    }
}

void Gpio_port_b_int_handler(void) {
    uint32_t status;

    /* Get the interrupt status of the GPIO */
    status = GPIOIntStatus(GPIO_PORTB_BASE, true);

    if (status) {
        /* Clear the Port B interrupt Status */
        GPIOIntClear(GPIO_PORTB_BASE, status);

        /* Loop through all the pins in port B */
        Gpio_handle_interrupt(status, 8, 16);
    }
}

void Gpio_port_c_int_handler(void) {
    uint32_t status;

    /* Get the interrupt status of the GPIO */
    status = GPIOIntStatus(GPIO_PORTC_BASE, true);

    if (status) {
        /* Clear the Port C interrupt Status */
        GPIOIntClear(GPIO_PORTC_BASE, status);

        /* Loop through all the pins in port C */
        Gpio_handle_interrupt(status, 16, 24);
    }
}

void Gpio_port_d_int_handler(void) {
    uint32_t status;

    /* Get the interrupt status of the GPIO */
    status = GPIOIntStatus(GPIO_PORTD_BASE, true);

    if (status) {
        /* Clear the Port D interrupt Status */
        GPIOIntClear(GPIO_PORTD_BASE, status);

        /* Loop through all the pins in port D */
        Gpio_handle_interrupt(status, 24, 32);
    }
}

void Gpio_port_e_int_handler(void) {
    uint32_t status;

    /* Get the interrupt status of the GPIO */
    status = GPIOIntStatus(GPIO_PORTE_BASE, true);

    if (status) {
        /* Clear the Port E interrupt Status */
        GPIOIntClear(GPIO_PORTE_BASE, status);

        /* Loop through all the pins in port E */
        Gpio_handle_interrupt(status, 32, 38);
    }
}

void Gpio_port_f_int_handler(void) {
    uint32_t status;
    /* Get the interrupt status of the GPIO */
    status = GPIOIntStatus(GPIO_PORTF_BASE, true);

    if (status) {
        /* Clear the Port F interrupt Status */
        GPIOIntClear(GPIO_PORTF_BASE, status);

        /* Loop through all the pins in port F */
        Gpio_handle_interrupt(status, 38, 43);
    }
}

ErrorCode Gpio_handle_interrupt(uint32_t gpio_int_status_in, GPIO_PIN_INDEX gpio_pin_lower_in, GPIO_PIN_INDEX gpio_pin_upper_in) {
    for (int i = gpio_pin_lower_in; i < gpio_pin_upper_in; ++i) {
        if (gpio_int_status_in & GPIO_PINS[i].interrupt_pin && GPIO_PINS[i].int_function != NULL) {
            /* Call the interrupt function pointer */
            DEBUG_DBG("Calling interrupt function");
            (*GPIO_PINS[i].int_function)();
        }
    }

    return ERROR_NONE;
}

ErrorCode Gpio_set_rising_interrupt(GPIO_PIN_INDEX gpio_id_number, void (*interrupt_callback)(void)) {
    Gpio_Module *p_gpio_pin = &GPIO_PINS[gpio_id_number];

    if (!p_gpio_pin->initialised) {
        DEBUG_ERR("Attempted to set rising edge interrupt on GPIO pin when\
        the GPIO has not been initialised");
        return GPIO_ERROR_NOT_INITIALISED;
    }
    
    if (gpio_id_number >= GPIO_NUM_GPIOS) {
        DEBUG_ERR("Gpio ID number has exceeded total number of GPIOs");
        return GPIO_ERROR_EXCEEDED_NUM_GPIOS;
    }

    GPIOIntTypeSet(p_gpio_pin->port, p_gpio_pin->pin, GPIO_RISING_EDGE);
    /* Set the interrupt type to rising edge interrupt */

    /* Set the interrupt function of the GPIO */
    p_gpio_pin->int_function = interrupt_callback;


    /* Check the port and call the appropriate function */
    switch (p_gpio_pin->port) {
        case GPIO_PORTA_BASE:
            DEBUG_INF("Registering interrupt for port A");
            GPIOIntRegister(p_gpio_pin->port, Gpio_port_a_int_handler);
            break;
        case GPIO_PORTB_BASE:
            DEBUG_INF("Registering interrupt for port B");
            GPIOIntRegister(p_gpio_pin->port, Gpio_port_b_int_handler);
            break;
        case GPIO_PORTC_BASE:
            DEBUG_INF("Registering interrupt for port C");
            GPIOIntRegister(p_gpio_pin->port, Gpio_port_c_int_handler);
            break;
        case GPIO_PORTD_BASE:
            DEBUG_INF("Registering interrupt for port D");
            GPIOIntRegister(p_gpio_pin->port, Gpio_port_d_int_handler);
            break;
        case GPIO_PORTE_BASE:
            DEBUG_INF("Registering interrupt for port E");
            GPIOIntRegister(p_gpio_pin->port, Gpio_port_e_int_handler);
            break;
        case GPIO_PORTF_BASE:
            DEBUG_INF("Registering interrupt for port F");
            GPIOIntRegister(p_gpio_pin->port, Gpio_port_f_int_handler);
            break;
        default:
            DEBUG_ERR("Unexpected GPIO port");
            return GPIO_ERROR_UNEXPECTED_PORT;
    }

    GPIOIntEnable(p_gpio_pin->port, p_gpio_pin->interrupt_pin);
    DEBUG_INF("GPIO rising interrupt has been set");
    return ERROR_NONE;
}

ErrorCode Gpio_reset_interrupt(GPIO_PIN_INDEX gpio_id_numer) {
    Gpio_Module *p_gpio_pin = &GPIO_PINS[gpio_id_numer];

    /* Check for valid gpio id number and initialisation before attempting
     * interrupt reset */
    if (!p_gpio_pin->initialised) {
        DEBUG_ERR("Attempted to set rising edge interrupt on GPIO pin when\
        the GPIO has not been initialised");
        return GPIO_ERROR_NOT_INITIALISED;
    }
    
    if (gpio_id_numer >= GPIO_NUM_GPIOS) {
        DEBUG_ERR("Gpio ID number has exceeded total number of GPIOs");
        return GPIO_ERROR_EXCEEDED_NUM_GPIOS;
    }

    /* Disable the gpio interrupt and unregister the port, as per the 
     * tiva manual */
    GPIOIntDisable(p_gpio_pin->port, p_gpio_pin->pin);
    GPIOIntUnregister(p_gpio_pin->port);
    p_gpio_pin->int_function = NULL;

    return ERROR_NONE;
}