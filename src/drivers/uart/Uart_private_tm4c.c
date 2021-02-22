/**
 * @ingroup uart
 * 
 * @file Uart_private_tm4c.c
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Private UART driver for the firmware.
 * 
 * Implements the UART driver for the TM4C.
 * 
 * @version 0.1
 * @date 2021-02-16
 * 
 * @copyright Copyright (c) 2021
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
#include "drivers/uart/Uart_public.h"
#include "drivers/uart/Uart_private.h"
#include "drivers/uart/Uart_errors.h"

/* TI Library Includes */
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

Uart_Device UART_PINS[UART_NUM_UARTS] = {
    {
        /* UART 1 (GNSS) */
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_UART0,
        GPIO_PORTA_BASE,
        UART0_BASE,
        GPIO_PA0_U0RX, /* TODO: Getting same error as I2C. Should still work */
        GPIO_PA1_U0TX,
        GPIO_PIN_0,
        GPIO_PIN_1,
        false
    },

    {
        /* UART 2 (CAM) */
        SYSCTL_PERIPH_GPIOC,
        SYSCTL_PERIPH_UART3,
        GPIO_PORTC_BASE,
        UART3_BASE,
        GPIO_PC6_U3RX, /* TODO: Getting same error as I2C. Should still work */
        GPIO_PC7_U3TX,
        GPIO_PIN_6,
        GPIO_PIN_7,
        false
    },

    {
        /* UART 3 (PWR) */
        SYSCTL_PERIPH_GPIOE,
        SYSCTL_PERIPH_UART7,
        GPIO_PORTE_BASE,
        UART7_BASE,
        GPIO_PE0_U7RX, /* TODO: Getting same error as I2C. Should still work */
        GPIO_PE1_U7TX,
        GPIO_PIN_0,
        GPIO_PIN_1,
        false
    },

    /* TODO: Add debugging UARTs */
};

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */