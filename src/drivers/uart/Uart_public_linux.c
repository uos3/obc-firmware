/**
 * @ingroup uart
 * 
 * @file Uart_public_linux.c
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public UART driver for the firmware.
 * 
 * Implements the UART driver for Linux.
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

#include "util/debug/Debug_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Uart_init(void) {
    DEBUG_DBG("UART init attempted when running on linux, ignoring.");
    return ERROR_NONE;
}

ErrorCode Uart_init_specific(Uart_DeviceId uart_id_in) {
    DEBUG_DBG("UART init specific attempted when running on linux, ignoring.");
    return ERROR_NONE;
}

ErrorCode Uart_send_bytes(
    Uart_DeviceId uart_id_in,
    uint8_t *p_data_in, 
    size_t length_in
) {
    DEBUG_DBG("UART send bytes attempted when running on linux, ignoring.");
    return ERROR_NONE;
}

ErrorCode Uart_recv_bytes(
    Uart_DeviceId uart_id_in,
    uint8_t *p_data_out,
    size_t length_in
) {
    DEBUG_DBG("UART recv bytes attempted when running on linux, ignoring.");
    return ERROR_NONE;
}

ErrorCode Uart_get_status(
    Uart_DeviceId uart_id_in,
    Uart_Status p_status_out
) {
    DEBUG_DBG("UART get status attempted when running on linux, ignoring.");
    return ERROR_NONE;
}
