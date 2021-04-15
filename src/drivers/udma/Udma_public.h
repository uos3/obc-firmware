/**
 * @ingroup udma
 * 
 * @file Udma_public.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public header for the uDMA driver.
 * 
 * @version 0.1
 * @date 2021-02-16
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_UDMA_PUBLIC_H
#define H_UDMA_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard Lirary */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Internal */
#include "drivers/uart/Uart_errors.h"
#include "drivers/uart/Uart_public.h"
#include "drivers/uart/Uart_private.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

extern bool UDMA_INITIALISED;
unsigned char UDMA_CONTROL_TABLE[1024];

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the uDMA for UART transfer. This initialisation contains
 * TI functions which are only used once or infrequently to set up the uDMA
 * channel. Transfer address, size, and transfer modes must be set using
 * uDMAChannelTransferSet and uDMAChannelEnable before each new transfer, but
 * are not contained within this init function. Therefore, this function
 * should only be called once (or infrequently if re-initialisation required).
 * 
 * @return ErrorCode 
 */
ErrorCode Udma_init(void);

/**
 * @brief Interrupt handler for the Udma
 * TODO: Unfinished
 * 
 * @param uart_id_in 
 * @param length_in 
 * @return ErrorCode If no error, ERROR_NONE, otherwise UDMA_ERROR_x.
 */
ErrorCode Udma_interrupt_handler(
    Uart_DeviceId uart_id_in,
    size_t length_in
);

#endif /* H_UDMA_PUBLIC_H */