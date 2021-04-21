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
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* Create an array of each GPIO module (with each port and each pin defined
 * within external includes)
 * 
 * Note from TivaWare manual: Ports ABCD each have 8 pins 0-7, but port E has
 * 6 pins 0-5, and port F has 5 pins 0-4. */

Gpio_Module GPIO_PINS[GPIO_MAX_NUM_GPIOS] = {
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, GPIO_MODE_NONE, false, NULL},

    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, GPIO_MODE_NONE, false, NULL},

    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, GPIO_MODE_NONE, false, NULL},

    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, GPIO_MODE_NONE, false, NULL},

    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, GPIO_MODE_NONE, false, NULL},

    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, GPIO_MODE_NONE, false, NULL},
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, GPIO_MODE_NONE, false, NULL},
} ;
