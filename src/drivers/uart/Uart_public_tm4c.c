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
#include "drivers/uart/Uart_events.h"
#include "drivers/gpio/Gpio_public.h"
#include "drivers/udma/Udma_errors.h"
#include "drivers/udma/Udma_public.h"
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"

/* External */
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/udma.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/interrupt.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Uart_init(void) {
    /* TODO: Check actions of function match doc comment */
    /* Call specific init functions for all Uart devices (GNSS, CAM, EPS in
     * that respective order, see UART DEVICE INDEX in Uart_public.h) */
    Uart_init_specific(UART_DEVICE_ID_GNSS);
    Uart_init_specific(UART_DEVICE_ID_CAM);
    Uart_init_specific(UART_DEVICE_ID_EPS);
    Uart_init_specific(UART_DEVICE_ID_TEST);

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

ErrorCode Uart_init_specific(Uart_DeviceId uart_id_in) {
    /* Check that the ID number of the UART is acceptable, return an error
     * if not. */
    if (uart_id_in >= UART_NUM_UARTS) {
        DEBUG_ERR("The UART ID number was greater than the number of UARTs");
        return UART_ERROR_MAX_NUM_UARTS;
    }

    /* Pointer to the UART */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];
    
    /* Initialise the GPIO pins as their respective mode */
    if (!SysCtlPeripheralReady(p_uart_device->gpio_peripheral)) {
        SysCtlPeripheralReset(p_uart_device->gpio_peripheral);
        SysCtlPeripheralEnable(p_uart_device->gpio_peripheral);

        for (int i = 0; i <= UART_MAX_NUM_PERIPHERAL_READY_CHECKS; ++i) {
            if (!SysCtlPeripheralReady(p_uart_device->gpio_peripheral) && i >= UART_MAX_NUM_PERIPHERAL_READY_CHECKS) {
                DEBUG_ERR("Failed to enable GPIO peripheral");
                return UART_ERROR_PERIPHERAL_READY_FAILED;
            }
        }
    }

    /* Check if the UART has already been initialised, give a warning if it has
     * already been initialised, or continue if it has not. */
    if (p_uart_device->initialised) {
        DEBUG_WRN("Uart_init called when already initialised");
    }

    /* Initialise the UART peripheral */
    if (!SysCtlPeripheralReady(p_uart_device->uart_peripheral)) {
        SysCtlPeripheralReset(p_uart_device->uart_peripheral);
        SysCtlPeripheralEnable(p_uart_device->uart_peripheral);
    }

    for (int i = 0; i <= UART_MAX_NUM_PERIPHERAL_READY_CHECKS; ++i) {
        if (!SysCtlPeripheralReady(p_uart_device->uart_peripheral) && i >= UART_MAX_NUM_PERIPHERAL_READY_CHECKS) {
            DEBUG_ERR("Failed to enable GPIO peripheral");
            return UART_ERROR_PERIPHERAL_READY_FAILED;
        }
    }

    /* Configure the GPIO pins */
    GPIOPinConfigure(p_uart_device->uart_pin_rx_func);
    GPIOPinConfigure(p_uart_device->uart_pin_tx_func);
    GPIOPinTypeUART(p_uart_device->gpio_base,
        p_uart_device->gpio_pin_rx | p_uart_device->gpio_pin_tx
    );

    /* Set the TX and RX FIFO trigger thresholds to tell the uDMA
        * controller when more data should be transferred. These are defined
        * in Uart_private.h and are currently arbitrary
        * (see TODO in Uart_private.h for more info). */
    UARTFIFOLevelSet(
        p_uart_device->uart_base, 
        UART_TX_FIFO_THRESHOLD, 
        UART_RX_FIFO_THRESHOLD
    );

    /* Enable the UART and uDMA interface for TX and RX */
    UARTEnable(p_uart_device->uart_base);
    UARTDMAEnable(p_uart_device->uart_base, UART_DMA_RX | UART_DMA_TX);
    
    UARTConfigSetExpClk(
        p_uart_device->uart_base,
        SysCtlClockGet(),
        p_uart_device->baud_rate,
        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE
    );

    /* Set the UART state as initialised. */
    p_uart_device->initialised = true;
        /* Return error none if this point has been reached without any errors
         * occuring. */
        return ERROR_NONE;
}

ErrorCode Uart_send_bytes(
    Uart_DeviceId uart_id_in,
    uint8_t *p_data_in, 
    uint32_t length_in
) {

    /* Check that the ID number of the UART is acceptable, return an error
     * if not. */
    if (uart_id_in >= UART_NUM_UARTS) {
        DEBUG_ERR("The UART ID number was greater than the number of UARTs");
        return UART_ERROR_MAX_NUM_UARTS;
    }

    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    if (!p_uart_device->initialised) {
        DEBUG_ERR("Attempted to send bytes while uDMA not initialised.");
        return UDMA_ERROR_NOT_INITIALISED;
    }

    /* Enable the UART interrupt.
     * TODO: Check this, and in rx */
    UARTIntEnable(p_uart_device->uart_base, UART_INT_DMATX);
    // IntEnable(p_uart_device->uart_base_int);

    switch(uart_id_in) {
        case UART_DEVICE_ID_CAM:
            UARTIntRegister(p_uart_device->uart_base, Uart_cam_tx_int_handler);
            // IntRegister(p_uart_device->uart_base_int, Uart_cam_tx_int_handler);
            break;
        case UART_DEVICE_ID_GNSS:
            UARTIntRegister(p_uart_device->uart_base, Uart_gnss_tx_int_handler);
            // IntRegister(p_uart_device->uart_base_int, Uart_gnss_tx_int_handler);
            break;
        case UART_DEVICE_ID_EPS:
            UARTIntRegister(p_uart_device->uart_base, Uart_eps_tx_int_handler);
            // IntRegister(p_uart_device->uart_base_int, Uart_eps_tx_int_handler);
            break;
        case UART_DEVICE_ID_TEST:
            UARTIntRegister(p_uart_device->uart_base, Uart_test_tx_int_handler);
            // IntRegister(p_uart_device->uart_base_int, Uart_test_tx_int_handler);
            break;
        default:
            DEBUG_ERR("Unexpected UART ID");
            return UART_ERROR_UNEXPECTED_DEVICE_ID;
    }

    uDMAChannelAssign(p_uart_device->udma_channel_tx);
    
    /* Configure the control parameters for the UART TX channel.
     * UDMA_ARB_4 to match the FIFO trigger threshold.
     * UDMA_ARB_4 refers to the arbitratoin size, which is the number
     * of bytes transferred per trigger. This may not be necessary in AUTO
     * mode, but if we decide to use BASIC mode, this value may be required,
     * so will be kept in for now. */
    uDMAChannelControlSet(
        p_uart_device->udma_channel_tx | UDMA_PRI_SELECT,
        UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_4
    );
    
    /* Set the transfer addresses, size, and mode for TX */
    uDMAChannelTransferSet(
        p_uart_device->udma_channel_tx | UDMA_PRI_SELECT,
        UDMA_MODE_AUTO,
        (void *)p_data_in,
        (void *)(p_uart_device->uart_base + UART_O_DR), /* TODO: Check this, and in rx */
        length_in
    );

    /* Enable the uDMA channel for the transfer to occur. */
    uDMAChannelEnable(p_uart_device->udma_channel_tx);

    p_uart_device->uart_status_tx = UART_STATUS_IN_PROGRESS;


    /* FIXME: uDMAErrorStatusGet */

    Event uart_event_tx;
    Event uart_event_rx;
    if (!Uart_get_events_for_device(uart_id_in, &uart_event_tx, &uart_event_rx)) {
        return UART_ERROR_EVENTS_FAILED;
    }
    p_uart_device->uart_event = uart_event_tx;

    return ERROR_NONE;
}

ErrorCode Uart_recv_bytes(
    Uart_DeviceId uart_id_in,
    uint8_t *p_data_out,
    uint32_t length_in
) {

    /* Check that the ID number of the UART is acceptable, return an error
     * if not. */
    if (uart_id_in >= UART_NUM_UARTS) {
        DEBUG_ERR("The UART ID number was greater than the number of UARTs");
        return UART_ERROR_MAX_NUM_UARTS;
    }

    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    if (!p_uart_device->initialised) {
        DEBUG_ERR("Attempted to send bytes while uDMA not initialised.");
        return UDMA_ERROR_NOT_INITIALISED;
    }

    UARTIntEnable(p_uart_device->uart_base, UART_INT_DMARX);
    // IntEnable(p_uart_device->uart_base_int);

    switch(uart_id_in) {
        case UART_DEVICE_ID_CAM:
            UARTIntRegister(p_uart_device->uart_base, Uart_cam_rx_int_handler);
            // IntRegister(p_uart_device->uart_base_int, Uart_cam_rx_int_handler);
            break;
        case UART_DEVICE_ID_GNSS:
            UARTIntRegister(p_uart_device->uart_base, Uart_gnss_rx_int_handler);
            // IntRegister(p_uart_device->uart_base_int, Uart_gnss_rx_int_handler);
            break;
        case UART_DEVICE_ID_EPS:
            UARTIntRegister(p_uart_device->uart_base, Uart_eps_rx_int_handler);
            // IntRegister(p_uart_device->uart_base_int, Uart_eps_rx_int_handler);
            break;
        case UART_DEVICE_ID_TEST:
            UARTIntRegister(p_uart_device->uart_base, Uart_test_rx_int_handler);
            // IntRegister(p_uart_device->uart_base_int, Uart_test_rx_int_handler);
            break;
        default:
            DEBUG_ERR("Unexpected UART device ID");
            return UART_ERROR_UNEXPECTED_DEVICE_ID;
    }

    uDMAChannelAssign(p_uart_device->udma_channel_tx);

    /* Configure the control parameters for the UART RX channel.
     * UDMA_ARB_4 to match the FIFO trigger threshold. 
     * TODO: make this the device specific rx channel */
    uDMAChannelControlSet(
        p_uart_device->udma_channel_rx | UDMA_PRI_SELECT,
        UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_4
    );
    
    /* Set the transfer addresses, size, and mode for RX */
    uDMAChannelTransferSet(
        p_uart_device->udma_channel_rx | UDMA_PRI_SELECT,
        UDMA_MODE_AUTO,
        (void *)(p_uart_device->uart_base + UART_O_DR),
        (void *)p_data_out,
        length_in
    );

    /* Enable the channel. Software-initiated transfers require a channel
     * request to begin the transfer. */
    uDMAChannelEnable(p_uart_device->udma_channel_rx);

    p_uart_device->uart_status_rx = UART_STATUS_IN_PROGRESS;

    /* Enable the UART interrupt.
     * TODO: Check this, and in rx */
    #if 0
    UARTIntDisable(p_uart_device->uart_base, UART_INT_RX | UART_INT_DMARX);
    UARTIntClear(p_uart_device->uart_base, UART_INT_RX | UART_INT_DMARX);
    UARTIntEnable(p_uart_device->uart_base, UART_INT_RX | UART_INT_DMARX);
    #endif
    

    /* FIXME: Check uDMAErrorStatusGet */

    Event uart_event_tx;
    Event uart_event_rx;
    if (!Uart_get_events_for_device(uart_id_in, &uart_event_tx, &uart_event_rx)) {
        return UART_ERROR_EVENTS_FAILED;
    }
    p_uart_device->uart_event = uart_event_rx;

    return ERROR_NONE;
}

ErrorCode Uart_get_status(Uart_DeviceId uart_id_in, Uart_Status p_status_out) {
    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    if (!p_uart_device->initialised) {
        DEBUG_ERR("Attempted to send bytes while uDMA not initialised.");
        return UDMA_ERROR_NOT_INITIALISED;
    }

    /* Check that the ID number of the UART is acceptable, return an error
     * if not. */
    if (uart_id_in >= UART_NUM_UARTS) {
        DEBUG_ERR("The UART ID number was greater than the number of UARTs");
        return UART_ERROR_MAX_NUM_UARTS;
    }

    switch(uDMAErrorStatusGet()) {
        case 0:
            p_status_out = UART_STATUS_COMPLETE;
            break;
        default:
            p_status_out = UART_STATUS_UDMA_TRANSFER_ERROR;
    }

    if (p_status_out == UART_STATUS_UDMA_TRANSFER_ERROR) {
        /* Check the uDMA error status, return an error if non-zero */
        DEBUG_WRN("uDMAErrorStatusGet returned a nonspecified non-zero error");
    }
    return ERROR_NONE;
}

ErrorCode Uart_step(void) {
    int i;
    for (i = 0; i < UART_NUM_UARTS; ++i) {
        /* Loop through all UART devices whose status is NOT UART_STATUS_NONE
         * or UART_STATUS_IN_PROGRESS */
        if (UART_DEVICES[i].uart_status_rx
        !=
        UART_STATUS_IN_PROGRESS
        &&
        UART_DEVICES[i].uart_status_tx
        !=
        UART_STATUS_IN_PROGRESS
        &&
        UART_DEVICES[i].uart_status_rx
        !=
        UART_STATUS_NONE
        &&
        UART_DEVICES[i].uart_status_tx
        !=
        UART_STATUS_NONE
        ) {
            /* Check to see if the UART device's event is raised. */
            if (!EventManager_is_event_raised(UART_DEVICES[i].uart_event)) {
                /* If the UART device's event is not raise, raise it. */
                EventManager_raise_event(UART_DEVICES[i].uart_event);
            }
        }
    }

    /* If this point has been reached without error, return ERROR NONE. */
    return ERROR_NONE;
}

bool Uart_get_events_for_device(
    Uart_DeviceId device_id_in,
    Event *p_tx_event_out,
    Event *p_rx_event_out
) {
    switch (device_id_in) {
        case UART_DEVICE_ID_CAM:
            *p_tx_event_out = EVT_UART_CAM_TX_COMPLETE;
            *p_rx_event_out = EVT_UART_CAM_RX_COMPLETE;
            break;
        case UART_DEVICE_ID_GNSS:
            *p_tx_event_out = EVT_UART_GNSS_TX_COMPLETE;
            *p_rx_event_out = EVT_UART_GNSS_RX_COMPLETE;
            break;
        case UART_DEVICE_ID_EPS:
            *p_tx_event_out = EVT_UART_EPS_TX_COMPLETE;
            *p_rx_event_out = EVT_UART_EPS_RX_COMPLETE;
            break;
        case UART_DEVICE_ID_TEST:
            *p_tx_event_out = EVT_UART_TEST_TX_COMPLETE;
            *p_rx_event_out = EVT_UART_TEST_RX_COMPLETE;
            break;
        default:
            DEBUG_ERR("Unexpected device ID");
            return UART_ERROR_UNEXPECTED_DEVICE_ID;
    }

    return true;
}

/* -------------------------------------------------------------------------   
 * FUNCTIONS BELOW ARE PART OF THE OLD INTERFACE. WILL MOST LIKELY BE DISCARED,
 * BUT KEPT FOR NOW AS A REFERENCE/JUST IN CASE UDMA IS NOT A VIABLE OPTION.
 * ------------------------------------------------------------------------- */

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

    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_TX);

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

    /* Check that the ID number of the UART is acceptable, return an error
     * if not. */
    if (uart_id_number_in >= UART_NUM_UARTS) {
        DEBUG_ERR("The UART ID number was greater than the number of UARTs");
        return UART_ERROR_MAX_NUM_UARTS;
    }
    
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_number_in];

    p_uart_device->uart_status_tx = UART_STATUS_IN_PROGRESS;

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
    while (buffer_length_in--) {
        if (UARTCharPutNonBlocking(p_uart_device->uart_base, *buffer_out++)) {
            /* This function returns true if the character was placed in the TX
             * FIFO, so if this point has been reached, return no error. */
        }
        else {
            /* The function returns false if there is no space available in the
             * TX FIFO, so raise an error. */
            DEBUG_ERR("No space available in specified UART port, failed to\
            send byte.");
            Debug_exit(1);
            return UART_ERROR_PUT_CHAR_FAILED;
        }
    }

    /* Return error none if this point has been reached without any errors. */
    p_uart_device->uart_status_tx = UART_STATUS_COMPLETE;
    return ERROR_NONE;
}