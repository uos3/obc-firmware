/**
 * @ingroup uart
 * 
 * @file Uart_public_tm4c.c
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public UART driver for the firmware.
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

#include "drivers/uart/Uart_private_tm4c.c"

/* External */
#include "driverlib/gpio.h" /* Might be needed later for UART */
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Uart_init(uint8_t uart_id_number_in) {
    /* Pointer to the UART */
    Uart_Device *p_uart_device = &UART_PINS[uart_id_number_in];

    /* Check that the ID number of the UART is acceptable, return an error
     * if not. */
    if (uart_id_number_in >= UART_NUM_UARTS) {
        DEBUG_ERR("The UART ID number was greater than the number of UARTs");
        return UART_ERROR_MAX_NUM_UARTS;
    }

    /* Check if the UART has already been initialised, give a warning if it has
     * already been initialised, or continue if it has not. */
    if (p_uart_device->initialised) {
        DEBUG_WRN("Uart_init called when already initialised");
    }
    else {
        /* Initialise the UART peripheral */
        if (!SysCtlPeripheralReady(p_uart_device->uart_peripheral)) {
            SysCtlPeripheralReset(p_uart_device->uart_peripheral);
            SysCtlPeripheralReady(p_uart_device->uart_peripheral);
        }

        for (int i = 0; i < UART_MAX_NUM_PERIPHERAL_READY_CHECKS; ++i) {
            if (SysCtlPeripheralReady(p_uart_device->uart_peripheral)) {
                /* If the peripheral is ready, break out of the loop */
                break;
            }
            if (i >= UART_MAX_NUM_PERIPHERAL_READY_CHECKS) {
                /* If the maximium number of peripheral ready checks has been
                 * reached, raise an error. */
                DEBUG_ERR("Failed to enable UART peripheral");
                return UART_ERROR_PERIPHERAL_READY_FAILED;
            }
        }

        /* Initialise the GPIO peripheral if not already */
        if (!SysCtlPeripheralReady(p_uart_device->gpio_peripheral)) {
            SysCtlPeripheralReset(p_uart_device->gpio_peripheral);
            SysCtlPeripheralReady(p_uart_device->gpio_peripheral);
        }

        for (int i = 0; i < UART_MAX_NUM_PERIPHERAL_READY_CHECKS; ++i) {
            if (SysCtlPeripheralReady(p_uart_device->gpio_peripheral)) {
                /* If the peripheral is ready, break out of the loop */
                break;
            }
            if (i >= UART_MAX_NUM_PERIPHERAL_READY_CHECKS) {
                /* If the maximium number of peripheral ready checks has been
                 * reached, raise an error. */
                DEBUG_ERR("Failed to enable UART peripheral");
                return UART_ERROR_PERIPHERAL_READY_FAILED;
            }
        }

        GPIOPinConfigure(p_uart_device->uart_pin_rx_func);
        GPIOPinConfigure(p_uart_device->uart_pin_tx_func);
        /* TODO: Check the bitwise OR is used correctly. The parameter is the
         * "bit-packed representation of the pins" from TI manual page 283. */
        GPIOPinTypeUART(p_uart_device->gpio_base,
            p_uart_device->gpio_pin_rx | p_uart_device->gpio_pin_tx
        );

        /* TODO: Set baud rate? (Or check what default value is, or what
         * value is required by UoS3) */
    }

    /* Set the UART state as initialised. */
    p_uart_device->initialised = true;
        /* Return error none if this point has been reached without any errors
         * occuring. */
        return ERROR_NONE;
}

ErrorCode Uart_get_char(uint8_t uart_id_number_in, bool is_nonblocking_in, char *recvd_byte_out) {
    Uart_Device *p_uart_device = &UART_PINS[uart_id_number_in];

    /* Check that the ID number of the UART is acceptable, return an error
     * if not. */
    if (uart_id_number_in >= UART_NUM_UARTS) {
        DEBUG_ERR("The UART ID number was greater than the number of UARTs");
        return UART_ERROR_MAX_NUM_UARTS;
    }

    /* Check that the UART has been initialised before continuing, return an
     * error if not. */
    if (!p_uart_device->initialised) {
        DEBUG_ERR(
            "Attempted to call Uart_get_char when UART has not been\
            initialised."
        );
        return UART_ERROR_NOT_INITIALISED;
    }

    /* If the function is NOT nonblocking, get the byte using TI UARTCharGet
     * function. */
    if (!is_nonblocking_in) {
        *recvd_byte_out = (char)UARTCharGet(p_uart_device->uart_base);
    }
    else {
        /* Check if there are any characters available in the receive FIFO. If
         * there are, get the character. If not, show a warning that no
         * characters are available. */
        if (UARTCharsAvail(p_uart_device->uart_base) != 0) {
            *recvd_byte_out = (char)UARTCharGetNonBlocking(p_uart_device->uart_base);
        }
        else {
            DEBUG_WRN("Uart_get_char attempted when there are no available\
             characters present in the receive FIFO."
            );
            return ERROR_NONE;
        }
    }

    /* Return error none if this point has been reached without any errors. */
    return ERROR_NONE;
}