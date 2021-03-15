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
#include "drivers/udma/Udma_errors.h"
#include "drivers/udma/Udma_public.h"

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

ErrorCode Uart_udma_init(void) {
    
    /* Check that the uDMA peripheral is ready, if not, then enable the
     * peripheral. */
    if (!SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA)) {
        SysCtlPeripheralReset(SYSCTL_PERIPH_UDMA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    }

    for (int i = 0; i < UART_MAX_NUM_PERIPHERAL_READY_CHECKS; ++i) {
            if (SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA)) {
                /* If the peripheral is ready, break out of the loop */
                break;
            }
            if (i >= UART_MAX_NUM_PERIPHERAL_READY_CHECKS) {
                /* If the maximium number of peripheral ready checks has been
                 * reached, raise an error. */
                DEBUG_ERR("Failed to enable uDMA peripheral");
                return UDMA_ERROR_PERIPHERAL_NOT_READY;
            }
        }

    /* Enable the uDMA channels */
    uDMAEnable();
    uDMAControlBaseSet(UDMA_CONTROL_TABLE);

    UDMA_INITIALISED = true;

    return ERROR_NONE;
}

ErrorCode Uart_udma_interrupt_handler(
    Uart_DeviceId uart_id_in,
    size_t length_in
) {
    /* Pointer to UART device */
    Uart_Device *p_uart_device = &UART_DEVICES[uart_id_in];

    /* Get the interrupt status of the UART */
    p_uart_device->uart_status = UARTIntStatus(p_uart_device->uart_base, true);
    /* Clear any pending UART status. No UART interrupts should be enabled, as
     * both RX and TX are using uDMA. */
    UARTIntClear(p_uart_device->uart_base, p_uart_device->uart_status);

    /* Check the uDMA control table to check that the transfer is complete */
    p_uart_device->udma_mode = uDMAChannelModeGet(p_uart_device->udma_channel_rx | UDMA_PRI_SELECT);

    /* The transfer is complete if the mode is "STOP" */
    if (p_uart_device->udma_mode == UDMA_MODE_STOP) {
        /* TODO: Count total number of complete transfers? */
    }
}


