/**
 * @ingroup uart
 * 
 * @file Uart_private.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Private header for the UART driver.
 * 
 * @version 0.1
 * @date 2021-02-16
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_UART_PRIVATE_H
#define H_UART_PRIVATE_H

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
#include "drivers/uart/Uart_errors.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/*
extern Uart_Device UART_DEVICES;
*/

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/* Number of UART devices. */
#define UART_NUM_UARTS (3)

/* Maximum number of times the peripheral ready check will occur before raising
 * an error.
 * TODO: This is currently arbitrary */
#define UART_MAX_NUM_PERIPHERAL_READY_CHECKS (10)

/* FIFO threshold levels for RX and TX to tell the uDMA when to begin the new
 * burst transfer. TODO: These are currently arbitrary, must check what
 * threshold would be most suitable. */
#define UART_TX_FIFO_THRESHOLD (UART_FIFO_TX4_8)
#define UART_RX_FIFO_THRESHOLD (UART_FIFO_RX4_8)

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

#endif /* H_UART_PRIVATE_H */