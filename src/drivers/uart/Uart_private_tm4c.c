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
 * DEFINES
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard Lirary */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Internal */
#include "util/debug/Debug_public.h"
#include "drivers/uart/Uart_public.h"
#include "drivers/uart/Uart_private.h"
#include "drivers/gpio/Gpio_public.h"
#include "system/event_manager/EventManager_public.h"

/* TI Library Includes */
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

Uart_Device UART_DEVICES[UART_NUM_UARTS] = {
    {
        /* UART ID 0 (GNSS, UART0) */
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_UART0,
        GPIO_PORTA_BASE,
        UART0_BASE,
        GPIO_PA0_U0RX, /* WAS GPIO_PA0_U0RX - RAISED AN ERROR */
        GPIO_PA1_U0TX, /* WAS GPIO_PA1_U0TX - RAISED AN ERROR */
        GPIO_PINA0,
        GPIO_PINA1,
        GPIO_PIN_0,
        GPIO_PIN_1,
        UDMA_CHANNEL_UART0RX,
        UDMA_CHANNEL_UART0TX,
        UART_STATUS_NONE,
        UART_STATUS_NONE,
        EVT_UART_GNSS_RX_COMPLETE,
        EVT_UART_GNSS_TX_COMPLETE,
        UDMA_MODE_STOP,
        INT_UART0,
        56000,
        false
    },

    {
        /* UART ID 1 (CAM) */
        SYSCTL_PERIPH_GPIOC,
        SYSCTL_PERIPH_UART3,
        GPIO_PORTC_BASE,
        UART3_BASE,
        GPIO_PC6_U3RX, /* GPIO_PC6_U3RX */
        GPIO_PC7_U3TX, /* GPIO_PC7_U3TX */
        GPIO_PINC6,
        GPIO_PINC7,
        GPIO_PIN_6,
        GPIO_PIN_7,
        UDMA_CH16_UART3RX,
        UDMA_CH17_UART3TX,
        UART_STATUS_NONE,
        UART_STATUS_NONE,
        EVT_UART_CAM_RX_COMPLETE,
        EVT_UART_CAM_TX_COMPLETE,
        UDMA_MODE_STOP,
        INT_UART3,
        115200,
        false
    },

    {
        /* UART ID 2 (EPS) */
        SYSCTL_PERIPH_GPIOE,
        SYSCTL_PERIPH_UART7,
        GPIO_PORTE_BASE,
        UART7_BASE,
        GPIO_PE0_U7RX, /* GPIO_PE0_U7RX */
        GPIO_PE1_U7TX, /* GPIO_PE1_U7TX */
        GPIO_PINE0,
        GPIO_PINE1,
        GPIO_PIN_0,
        GPIO_PIN_1,
        UDMA_CH20_UART7RX,
        UDMA_CH21_UART7TX, /* TODO: Check TI channel assignments */
        UART_STATUS_NONE,
        UART_STATUS_NONE,
        EVT_UART_EPS_RX_COMPLETE,
        EVT_UART_EPS_TX_COMPLETE,
        UDMA_MODE_STOP,
        INT_UART7,
        56000,
        false
    },

    {
        /* UART ID 3 (TEST) */
        SYSCTL_PERIPH_GPIOC,
        SYSCTL_PERIPH_UART1,
        GPIO_PORTC_BASE,
        UART1_BASE,
        GPIO_PC4_U1RX,
        GPIO_PC5_U1TX,
        GPIO_PINC4, /* use tivaware gpio not driver */
        GPIO_PINC5,
        GPIO_PIN_4,
        GPIO_PIN_5,
        UDMA_CHANNEL_UART1RX,
        UDMA_CHANNEL_UART1TX, /* TODO: Check TI channel assignments */
        UART_STATUS_NONE,
        UART_STATUS_NONE,
        EVT_UART_TEST_RX_COMPLETE,
        EVT_UART_TEST_TX_COMPLETE,
        UDMA_MODE_STOP,
        INT_UART1,
        115200,
        false
    },
};

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void Uart_gnss_int_handler(void) {
    Uart_service_irq(UART_DEVICE_ID_GNSS);
}


void Uart_cam_int_handler(void) {
    Uart_service_irq(UART_DEVICE_ID_CAM);
}


void Uart_eps_int_handler(void) {
    Uart_service_irq(UART_DEVICE_ID_EPS);
}


void Uart_test_int_handler(void) {
    Uart_service_irq(UART_DEVICE_ID_TEST);
}


/* service interrupt requuest for interrupt */
void Uart_service_irq(Uart_DeviceId uart_id_in) {
    uint32_t udma_mode_tx;
    uint32_t udma_mode_rx;

    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    if (p_uart_device->uart_status_tx == UART_STATUS_IN_PROGRESS) {
        UARTIntClear(p_uart_device->uart_base, (UART_INT_TX | UART_INT_DMATX));
        udma_mode_tx = uDMAChannelModeGet(
            p_uart_device->udma_channel_tx | UDMA_PRI_SELECT
        );
        if (udma_mode_tx == UDMA_MODE_STOP) {
            EventManager_raise_event(p_uart_device->tx_event);
            Uart_get_status(uart_id_in, p_uart_device->uart_status_tx);
            /* TODO: Change name of the above func to udma get status
             * and move to udma module */
        }
    }
    if (p_uart_device->uart_status_rx == UART_STATUS_IN_PROGRESS) {
        UARTIntClear(p_uart_device->uart_base, (UART_INT_RX | UART_INT_DMARX));
        udma_mode_rx = uDMAChannelModeGet(
            p_uart_device->udma_channel_rx | UDMA_PRI_SELECT
        );
        if (udma_mode_rx == UDMA_MODE_STOP) {
            EventManager_raise_event(p_uart_device->rx_event);
            Uart_get_status(uart_id_in, p_uart_device->uart_status_rx);
            /* TODO: Change name of the above func to udma get status
             * and move to udma module */
        }
    }
}

void Uart_service_tx_irq(Uart_DeviceId uart_id_in) {

    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    /* Clear any pending UART status. No UART interrupts should be enabled,
    * as both RX and TX are using uDMA. */
    UARTIntClear(p_uart_device->uart_base, (UART_INT_TX | UART_INT_DMATX));

    /* Check the uDMA control table to check that the transfer is complete */
    p_uart_device->udma_mode = uDMAChannelModeGet(
        p_uart_device->udma_channel_tx | UDMA_PRI_SELECT
    );

    /* The transfer is complete if the TX mode is "UDMA_MODE_STOP", so
    * raise the event and set the TX status as complete. */
    if (p_uart_device->udma_mode == UDMA_MODE_STOP) {
        EventManager_raise_event(p_uart_device->tx_event);
        p_uart_device->uart_status_tx = UART_STATUS_COMPLETE;
    }
}
