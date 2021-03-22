/**
 * @file test_uart.c
 * @author Leon Galanakis (leongalanakis@gmail.com)
 * @brief uDMA UART test
 * 
 * 
 * 
 * @version 0.1
 * @date 2021-03-16
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "drivers/uart/Uart_private.h"
#include "drivers/uart/Uart_public.h"
#include "drivers/udma/Udma_public.h"
#include "util/debug/Debug_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
    int i;
    size_t data_size = 8;
    uint8_t send_data[data_size];
    uint8_t recv_data[data_size];
    /* TODO: Unfinished */

    /* Initialise the UART devices. */
    if (Uart_init() != ERROR_NONE) {
        DEBUG_ERR("Failed to initialise the UART devices.");
    }

    /* Initialise the uDMA */
    if (Udma_init() != ERROR_NONE) {
        DEBUG_ERR("Failed to initialise the uDMA.");
    }

    /* Loop through the size of the data to send, and create an array of
     * dummy data to be sent. */
    for (i = 0; i < data_size; ++i) {
        send_data[i] = i;
    }

    /* Send the data to the UART RX associated with the UART device.
     * TODO: Change which device for send and receive below. */
    if (Uart_send_bytes(UART_DEVICE_ID_CAM, send_data, data_size) != ERROR_NONE) {
        DEBUG_ERR("Failed to send bytes to UART device.");
    }

    /* Receive the data from the UART TX associated with the UART device. */
    if (Uart_recv_bytes(UART_DEVICE_ID_CAM, recv_data, data_size) != ERROR_NONE) {
        DEBUG_ERR("Failed to receive bytes from UART device.");
    }

    /* Loop through the data size, making sure that the data received from the
     * TX is the same as what was sent to the RX (the raspberry pi should re
     * echo the data it received, and send the same data back) */
    for (i = 0; i < data_size; ++i) {
        if (recv_data[i] != send_data[i]) {
            DEBUG_ERR("Unexpected receive data - should be the same as sent.");
            break;
        }
    }

    /* Return 0 if no errors occured up to this point. */
    return 0;
}