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
#include "drivers/uart/Uart_events.h"
#include "system/event_manager/EventManager_events.h"

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
#define UART_NUM_UARTS (4)

/* Maximum number of times the peripheral ready check will occur before raising
 * an error.
 * TODO: This is currently arbitrary */
#define UART_MAX_NUM_PERIPHERAL_READY_CHECKS (10)

/* FIFO threshold levels for RX and TX to tell the uDMA when to begin the new
 * burst transfer. TODO: These are currently arbitrary, must check what
 * threshold would be most suitable. */
#define UART_TX_FIFO_THRESHOLD (UART_FIFO_TX4_8)
#define UART_RX_FIFO_THRESHOLD (UART_FIFO_RX4_8)

Uart_Device UART_DEVICES[UART_NUM_UARTS];


/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Uart GNSS RX interrupt handler
 * 
 * Gets the status of the transfer, and sets this onto the GNSS device's RX.
 * 
 */
void Uart_gnss_rx_int_handler(void);
/**
 * @brief Uart GNSS TX interrupt handler
 * 
 * Gets the status of the transfer, and sets this onto the GNSS device's TX.
 * 
 */
void Uart_gnss_tx_int_handler(void);

/**
 * @brief Uart CAM RX interrupt handler
 * 
 * Gets the status of the transfer, and sets this onto the CAM device's RX.
 * 
 */
void Uart_cam_rx_int_handler(void);
/**
 * @brief Uart CAM TX interrupt handler
 * 
 * Gets the status of the transfer, and sets this onto the CAM device's TX.
 * 
 */
void Uart_cam_tx_int_handler(void);

/**
 * @brief Uart EPS RX interrupt handler
 * 
 * Gets the status of the transfer, and sets this onto the EPS device's RX.
 * 
 */
void Uart_eps_rx_int_handler(void);
/**
 * @brief Uart EPS TX interrupt handler
 * 
 * Gets the status of the transfer, and sets this onto the EPS device's TX.
 * 
 */
void Uart_eps_tx_int_handler(void);

/**
 * @brief Uart TEST RX interrupt handler
 * 
 * Gets the status of the transfer, and sets this onto the TEST device's RX.
 * 
 */
void Uart_test_rx_int_handler(void);
/**
 * @brief Uart TEST TX interrupt handler
 * 
 * Gets the status of the transfer, and sets this onto the TEST device's TX.
 * 
 */
void Uart_test_tx_int_handler(void);

ErrorCode Udma_interrupt_handler_rx(
    Uart_DeviceId uart_id_in,
    size_t length_in
);

    ErrorCode Udma_interrupt_handler_rx(
    Uart_DeviceId uart_id_in,
    size_t length_in
);


#endif /* H_UART_PRIVATE_H */