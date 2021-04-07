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
#include "drivers/uart/Uart_events.h"
#include "drivers/gpio/Gpio_public.h"
#include "drivers/uart/Uart_events.h"
#include "system/event_manager/EventManager_public.h"

/* TI Library Includes */
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/udma.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

Uart_Device UART_DEVICES[UART_NUM_UARTS] = {
    {
        /* UART ID 0 (GNSS, UART0) */
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_UART0,
        GPIO_PORTA_BASE,
        UART0_BASE,
        GPIO_PA0_U0RX, /* TODO: Getting same error as I2C. Should still work */
        GPIO_PA1_U0TX,
        GPIO_PIN_0,
        GPIO_PIN_1,
        UDMA_CHANNEL_UART0TX,
        UDMA_CHANNEL_UART0RX,
        UART_STATUS_NONE,
        UART_STATUS_NONE,
        EVT_UART_NONE,
        UDMA_MODE_STOP,
        9600,
        false
    },

    {
        /* UART ID 1 (CAM) */
        SYSCTL_PERIPH_GPIOC,
        SYSCTL_PERIPH_UART3,
        GPIO_PORTC_BASE,
        UART3_BASE,
        GPIO_PC6_U3RX, /* TODO: Getting same error as I2C. Should still work */
        GPIO_PC7_U3TX,
        GPIO_PIN_6,
        GPIO_PIN_7,
        UDMA_CHANNEL_UART1TX,
        UDMA_CHANNEL_UART1RX,
        UART_STATUS_NONE,
        UART_STATUS_NONE,
        EVT_UART_NONE,
        UDMA_MODE_STOP,
        9600,
        false
    },

    {
        /* UART ID 2 (EPS) */
        SYSCTL_PERIPH_GPIOE,
        SYSCTL_PERIPH_UART7,
        GPIO_PORTE_BASE,
        UART7_BASE,
        GPIO_PE0_U7RX, /* TODO: Getting same error as I2C. Should still work */
        GPIO_PE1_U7TX,
        GPIO_PIN_0,
        GPIO_PIN_1,
        UDMA_CH1_UART2TX, /* TODO: Check TI channel assignments */
        UDMA_CH0_UART2RX,
        UART_STATUS_NONE,
        UART_STATUS_NONE,
        EVT_UART_NONE,
        UDMA_MODE_STOP,
        9600,
        false
    },

    {
        /* UART ID 3 (TEST) */
        SYSCTL_PERIPH_GPIOB,
        SYSCTL_PERIPH_UART0,
        GPIO_PORTB_BASE,
        UART0_BASE,
        GPIO_PCTL_PB0_U1RX,
        GPIO_PCTL_PB1_U1TX,
        GPIO_PIN_0, /* use tivaware gpio not driver */
        GPIO_PIN_1,
        UDMA_CHANNEL_UART0TX, /* TODO: Check TI channel assignments */
        UDMA_CHANNEL_UART0RX,
        UART_STATUS_NONE,
        UART_STATUS_NONE,
        EVT_UART_NONE,
        UDMA_MODE_STOP,
        9600,
        false
    },
};

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void Uart_gnss_rx_int_handler(void) {
    Uart_Status p_status_out = UART_STATUS_NONE;

    /* Get the status of the uDMA transfer (error or success) */
    Uart_get_status(UART_DEVICE_ID_GNSS, p_status_out);

    /* Store the status as a Uart_Status on the UART device (RX Status) */
    UART_DEVICES[UART_DEVICE_ID_GNSS].uart_status_rx = p_status_out;
    if (p_status_out == UART_STATUS_COMPLETE) {
        EventManager_raise_event(EVT_UART_GNSS_RX_COMPLETE);
    }
    /* store event raised flag? */
}
void Uart_gnss_tx_int_handler(void) {
    Uart_Status p_status_out = UART_STATUS_NONE;

    /* Get the status of the uDMA transfer (error or success) */
    Uart_get_status(UART_DEVICE_ID_GNSS, p_status_out);

    /* Store the status as a Uart_Status on the UART device (TX Status) */
    UART_DEVICES[UART_DEVICE_ID_GNSS].uart_status_tx = p_status_out;
    if (p_status_out == UART_STATUS_COMPLETE) {
        EventManager_raise_event(EVT_UART_GNSS_TX_COMPLETE);
    }
    /* store event raised flag? */
}

void Uart_cam_rx_int_handler(void) {
    Uart_Status p_status_out = UART_STATUS_NONE;

    /* Get the status of the uDMA transfer (error or success) */
    Uart_get_status(UART_DEVICE_ID_CAM, p_status_out);

    /* Store the status as a Uart_Status on the UART device (RX Status) */
    UART_DEVICES[UART_DEVICE_ID_CAM].uart_status_rx = p_status_out;
    if (p_status_out == UART_STATUS_COMPLETE) {
        EventManager_raise_event(EVT_UART_CAM_RX_COMPLETE);
    }
    /* store event raised flag? */
}
void Uart_cam_tx_int_handler(void) {
    Uart_Status p_status_out = UART_STATUS_NONE;

    /* Get the status of the uDMA transfer (error or success) */
    Uart_get_status(UART_DEVICE_ID_CAM, p_status_out);

    /* Store the status as a Uart_Status on the UART device (TX Status) */
    UART_DEVICES[UART_DEVICE_ID_CAM].uart_status_tx = p_status_out;
    if (p_status_out == UART_STATUS_COMPLETE) {
        EventManager_raise_event(EVT_UART_CAM_TX_COMPLETE);
    }
    /* store event raised flag? */
}

void Uart_eps_rx_int_handler(void) {
    Uart_Status p_status_out = UART_STATUS_NONE;

    /* Get the status of the uDMA transfer (error or success) */
    Uart_get_status(UART_DEVICE_ID_EPS, p_status_out);

    /* Store the status as a Uart_Status on the UART device (RX Status) */
    UART_DEVICES[UART_DEVICE_ID_EPS].uart_status_rx = p_status_out;
    if (p_status_out == UART_STATUS_COMPLETE) {
        EventManager_raise_event(EVT_UART_EPS_RX_COMPLETE);
    }
    /* store event raised flag? */
}
void Uart_eps_tx_int_handler(void) {
    Uart_Status p_status_out = UART_STATUS_NONE;

    /* Get the status of the uDMA transfer (error or success) */
    Uart_get_status(UART_DEVICE_ID_EPS, p_status_out);

    /* Store the status as a Uart_Status on the UART device (TX Status) */
    UART_DEVICES[UART_DEVICE_ID_EPS].uart_status_tx = p_status_out;
    if (p_status_out == UART_STATUS_COMPLETE) {
        EventManager_raise_event(EVT_UART_EPS_TX_COMPLETE);
    }
    /* store event raised flag? */
}

void Uart_test_rx_int_handler(void) {
    Uart_Status p_status_out = UART_STATUS_NONE;

    /* Get the status of the uDMA transfer (error or success) */
    Uart_get_status(UART_DEVICE_ID_TEST, p_status_out);

    /* Store the status as a Uart_Status on the UART device (RX Status) */
    UART_DEVICES[UART_DEVICE_ID_TEST].uart_status_rx = p_status_out;
    if (p_status_out == UART_STATUS_COMPLETE) {
        EventManager_raise_event(EVT_UART_TEST_RX_COMPLETE);
    }
    /* store event raised flag? */
}
void Uart_test_tx_int_handler(void) {
    Uart_Status p_status_out = UART_STATUS_NONE;

    /* Get the status of the uDMA transfer (error or success) */
    Uart_get_status(UART_DEVICE_ID_TEST, p_status_out);

    /* Store the status as a Uart_Status on the UART device (TX Status) */
    UART_DEVICES[UART_DEVICE_ID_TEST].uart_status_tx = p_status_out;
    if (p_status_out == UART_STATUS_COMPLETE) {
        EventManager_raise_event(EVT_UART_TEST_TX_COMPLETE);
    }
    /* store event raised flag? */
}

ErrorCode Udma_interrupt_handler_tx(
    Uart_DeviceId uart_id_in,
    size_t length_in
) {
    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    /* Get the interrupt status of the UART */
    p_uart_device->uart_status_tx
    =
    UARTIntStatus(
        p_uart_device->uart_base, true
    );
    /* Clear any pending UART status. No UART interrupts should be enabled, as
     * both RX and TX are using uDMA. */
    UARTIntClear(p_uart_device->uart_base, p_uart_device->uart_status_tx);

    /* Check the uDMA control table to check that the transfer is complete */
    p_uart_device->udma_mode
    =
    uDMAChannelModeGet(
        p_uart_device->udma_channel_tx | UDMA_PRI_SELECT
    );

    /* The transfer is complete if the mode is "STOP" */
    if (p_uart_device->udma_mode == UDMA_MODE_STOP) {
        switch (uart_id_in) {
            case UART_DEVICE_ID_GNSS:
                EventManager_raise_event(EVT_UART_GNSS_TX_COMPLETE);
                p_uart_device->uart_event = EVT_UART_GNSS_TX_COMPLETE;
                p_uart_device->uart_status_tx = UART_STATUS_COMPLETE;
            case UART_DEVICE_ID_CAM:
                EventManager_raise_event(EVT_UART_CAM_TX_COMPLETE);
                p_uart_device->uart_event = EVT_UART_CAM_TX_COMPLETE;
                p_uart_device->uart_status_tx = UART_STATUS_COMPLETE;
            case UART_DEVICE_ID_EPS:
                EventManager_raise_event(EVT_UART_EPS_TX_COMPLETE);
                p_uart_device->uart_event = EVT_UART_EPS_TX_COMPLETE;
                p_uart_device->uart_status_tx = UART_STATUS_COMPLETE;
            case UART_DEVICE_ID_TEST:
                EventManager_raise_event(EVT_UART_TEST_TX_COMPLETE);
                p_uart_device->uart_event = EVT_UART_TEST_TX_COMPLETE;
                p_uart_device->uart_status_tx = UART_STATUS_COMPLETE;
            default:
                return UART_ERROR_UNEXPECTED_DEVICE_ID;
        }
    }
    return ERROR_NONE;
}

ErrorCode Udma_interrupt_handler_rx(
    Uart_DeviceId uart_id_in,
    size_t length_in
) {
    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    /* Get the interrupt status of the UART */
    p_uart_device->uart_status_rx
    =
    UARTIntStatus(
        p_uart_device->uart_base, true
    );
    /* Clear any pending UART status. No UART interrupts should be enabled, as
     * both RX and TX are using uDMA. */
    UARTIntClear(p_uart_device->uart_base, p_uart_device->uart_status_rx);

    /* Check the uDMA control table to check that the transfer is complete */
    p_uart_device->udma_mode
    =
    uDMAChannelModeGet(
        p_uart_device->udma_channel_rx | UDMA_PRI_SELECT
    );

    /* The transfer is complete if the mode is "STOP" */
    if (p_uart_device->udma_mode == UDMA_MODE_STOP) {
        switch (uart_id_in) {
            case UART_DEVICE_ID_GNSS:
                EventManager_raise_event(EVT_UART_GNSS_RX_COMPLETE);
                p_uart_device->uart_event = EVT_UART_GNSS_RX_COMPLETE;
                p_uart_device->uart_status_rx = UART_STATUS_COMPLETE;
            case UART_DEVICE_ID_CAM:
                EventManager_raise_event(EVT_UART_CAM_RX_COMPLETE);
                p_uart_device->uart_event = EVT_UART_CAM_RX_COMPLETE;
                p_uart_device->uart_status_rx = UART_STATUS_COMPLETE;
            case UART_DEVICE_ID_EPS:
                EventManager_raise_event(EVT_UART_EPS_RX_COMPLETE);
                p_uart_device->uart_event = EVT_UART_EPS_RX_COMPLETE;
                p_uart_device->uart_status_rx = UART_STATUS_COMPLETE;
            case UART_DEVICE_ID_TEST:
                EventManager_raise_event(EVT_UART_TEST_RX_COMPLETE);
                p_uart_device->uart_event = EVT_UART_TEST_RX_COMPLETE;
                p_uart_device->uart_status_rx = UART_STATUS_COMPLETE;
            default:
                return UART_ERROR_UNEXPECTED_DEVICE_ID;
        }
    }
    return ERROR_NONE;
}
