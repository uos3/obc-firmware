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
#include "drivers/udma/Udma_errors.h"
#include "drivers/udma/Udma_public.h"

#include "util/debug/Debug_public.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Udma_init(void) {
    DEBUG_DBG("uDMA init attempted when running on linux, ignoring.");
    return ERROR_NONE;
}

ErrorCode Udma_interrupt_handler(
    Uart_DeviceId uart_id_in,
    size_t length_in
) {
    DEBUG_DBG("uDMA int handler attempted when running on linux, ignoring.");
    return ERROR_NONE;
}
