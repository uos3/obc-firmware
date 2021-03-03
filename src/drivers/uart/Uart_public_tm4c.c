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
#include "drivers/gpio/Gpio_public.h"

#include "drivers/gpio/Gpio_public_tm4c.c"

#include "drivers/uart/Uart_private_tm4c.c"

/* External */
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/udma.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Uart_init(void) {
    /* Call specific init functions for all Uart devices (GNSS, CAM, PWR in
     * that respective order, see UART DEVICE INDEX in Uart_public.h) */
    Uart_init_specific(UART_DEVICE_ID_GNSS);
    Uart_init_specific(UART_DEVICE_ID_CAM);
    Uart_init_specific(UART_DEVICE_ID_PWR);

    for (int i = 0; i < UART_NUM_UARTS; ++i) {
        /* Pointer to the UART */
        Uart_Device *p_uart_device = &UART_DEVICES[i];
        /* Looping through the UART devices, call a warning if the specific
         * UART failed to initialise, and continue.
         * In the case of 1 (or multiple) devices failing to initialise,
         * Uart_init_specific can be called to initialise that device. */
        if (!p_uart_device->initialised) {
            DEBUG_WRN("Warning: UART device index %d failed to initialise", i);
        }
    }

    return ERROR_NONE;
}

ErrorCode Uart_init_specific(Uart_DeviceId uart_id_number_in) {
    /* Pointer to the UART */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_number_in];
    uint32_t gpio_pins_init[2];

    Gpio_init(p_uart_device->gpio_pin_tx, 1, GPIO_MODE_INPUT);
    Gpio_init(p_uart_device->gpio_pin_rx, 1, GPIO_MODE_OUTPUT);

    GPIOPinConfigure(p_uart_device->uart_pin_rx_func);
        GPIOPinConfigure(p_uart_device->uart_pin_tx_func);
        /* TODO: Check the bitwise OR is used correctly. The parameter is the
         * "bit-packed representation of the pins" from TI manual page 283. */
        GPIOPinTypeUART(p_uart_device->gpio_base,
            p_uart_device->gpio_pin_rx | p_uart_device->gpio_pin_tx
        );

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

        /* TODO: Set baud rate? (Or check what default value is, or what
         * value is required by UoS3) */
    }

    /* Set the UART state as initialised. */
    p_uart_device->initialised = true;
        /* Return error none if this point has been reached without any errors
         * occuring. */
        return ERROR_NONE;
}

ErrorCode Uart_udma_init(void) {
    uint8_t udma_control_table[1024];
    
    if (!SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA)) {
        DBG_ERR("Attempted to initialise uDMA while peripheral not ready.");
        return UART_ERROR_UDMA_PERIPHERAL_NOT_READY;
    }

    uDMAEnable();
    uDMAControlBaseSet(udma_control_table);

    udma_initialised = true;

    return ERROR_NONE;
}

Uart_udma_interrupt_handler(
    Uart_DeviceId uart_id_in,
    size_t length_in
) {
    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    p_uart_device->uart_status = UARTIntStatus(p_uart_device->uart_base, true);
    p_uart_device->udma_mode = uDMAChannelModeGet(UDMA_CHANNEL_UART0RX | UDMA_PRI_SELECT);

    UARTIntClear(p_uart_device->uart_base, p_uart_device->uart_status);

    if (p_uart_device->udma_mode == UDMA_MODE_STOP) {
        /* TODO: Count RX pings? */

        uDMAChannelTransferSet(UDMA_CHANNEL_UART0RX | UDMA_PRI_SELECT,
            UDMA_MODE_AUTO,
            p_uart_device->gpio_pin_rx,
            p_uart_device->gpio_pin_tx,
            length_in);
    }

    if (!uDMAChannelIsEnabled(UDMA_CHANNEL_UART0TX)) {
        uDMAChannelTransferSet(UDMA_CHANNEL_UART0TX | UDMA_PRI_SELECT,
            UDMA_MODE_AUTO,
            p_uart_device->gpio_pin_tx,
            p_uart_device->gpio_pin_rx,
            length_in
        );

        uDMAChannelEnable(UDMA_CHANNEL_UART0TX);
    }
}

ErrorCode Uart_send_bytes(
    Uart_DeviceId uart_id_in,
    uint8_t *p_data_in, 
    size_t length_in
) {
    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    if (!udma_initialised) {
        DBG_ERR("Attempted to send bytes while uDMA not initialised.");
        return UART_ERROR_UDMA_NOT_INITIALISED;
    }

    /* Check that the ID number of the UART is acceptable, return an error
     * if not. */
    if (uart_id_in >= UART_NUM_UARTS) {
        DEBUG_ERR("The UART ID number was greater than the number of UARTs");
        return UART_ERROR_MAX_NUM_UARTS;
    }

    UARTEnable(p_uart_device->uart_base);
    UARTDMAEnable(p_uart_device->uart_base, UART_DMA_RX | UART_DMA_TX);

    uDMAChannelAttributeDisable(UDMA_CHANNEL_UART0TX,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK
    );

    uDMAChannelControlSet(UDMA_CHANNEL_UART0RX | UDMA_PRI_SELECT, UDMA_SIZE_8);
    
    /* Set the transfer addresses, size, and mode. */
    uDMAChannelTransferSet(UDMA_CHANNEL_UART0RX | UDMA_PRI_SELECT,
        UDMA_MODE_AUTO,
        p_uart_device->gpio_pin_rx,
        p_uart_device->gpio_pin_tx,
        length_in
    );

    /* Enable the channel. Software-initiated transfers require a channel
     * request to begin the transfer.
     * TODO: Check this */
    uDMAChannelEnable(UDMA_CHANNEL_SW);
    uDMAChannelRequest(UDMA_CHANNEL_SW);

    if (uDMAErrorStatusGet() != 0) {
        /* Check the uDMA error status, return an error if non-zero */
        DBG_ERR("Unknown uDMA error");
        /* TODO: Return an error */
    }
    else {
        /* if uDMAErrorStatusGet() returns a 0, no error is pending, so return
         * ERROR_NONE. */
        return ERROR_NONE;
    }
}

ErrorCode Uart_get_char(uint8_t uart_id_number_in, char *recvd_byte_out) {
    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_number_in];

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

    /* Return error none if this point has been reached without any errors. */
    return ERROR_NONE;
}

ErrorCode Uart_put_char(uint8_t uart_id_number_in, char byte_out) {
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_number_in];

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

    if (UARTCharPutNonBlocking(p_uart_device->uart_base, byte_out)) {
        /* This function returns true if the character was placed in the TX
         * FIFO, so if this point has been reached, return no error. */
        return ERROR_NONE;
    }
        else {
        /* The function returns false if there is no space available in the
         * TX FIFO, so raise an error. */
        DEBUG_ERR("No space available in specified UART port, failed to\
        send byte.");
        return UART_ERROR_PUT_CHAR_FAILED;
    }

    /* Return error none if this point has been reached without any errors. */
    return ERROR_NONE;
}

ErrorCode Uart_put_buffer(uint8_t uart_id_number_in, size_t buffer_length_in, char *buffer_out) {
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_number_in];

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

    /* TODO: Have another think about this. If space runs out halfway through
     * this loop, part of the buffer will have been sent. Would it be better to
     * send none of it unless there is sufficient space? Check to see if 
     * UARTCharsAvail would be suitable to use here.
     * Would implement roughly like:
     * if (UartCharsAvail) {
     * do the rest
     * } */
    for (int i = 0; i < buffer_length_in; ++i) {
        if (UARTCharPutNonBlocking(p_uart_device->uart_base, buffer_out[i])) {
            /* This function returns true if the character was placed in the TX
             * FIFO, so if this point has been reached, return no error. */
        }
        else {
            /* The function returns false if there is no space available in the
             * TX FIFO, so raise an error. */
            DEBUG_ERR("No space available in specified UART port, failed to\
            send byte.");
            return UART_ERROR_PUT_CHAR_FAILED;
        }
    }

    /* Return error none if this point has been reached without any errors. */
    return ERROR_NONE;
}