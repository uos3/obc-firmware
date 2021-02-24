/**
 * @ingroup uart
 * 
 * @file Uart_public.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public header for the UART driver.
 * 
 * @version 0.1
 * @date 2021-02-16
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_UART_PUBLIC_H
#define H_UART_PUBLIC_H

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

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */
/**
 * @brief Defines a UART.
 * 
 */
typedef struct _Uart_Device {
    uint32_t gpio_peripheral;
    uint32_t uart_peripheral;
    uint32_t gpio_base;
    uint32_t uart_base;
    uint32_t uart_pin_rx_func;
    uint32_t uart_pin_tx_func;
    uint8_t gpio_pin_rx;
    uint8_t gpio_pin_tx;
    bool initialised;
} Uart_Device;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialises the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @return ErrorCode 
 */
ErrorCode Uart_init(uint8_t uart_id_number_in);

/**
 * @brief Receieves a byte from the RX FIFO of the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @param is_blocking_in bool to check if the function should be nonblocking.
 * @param recvd_byte_out Byte received from the UART peripheral.
 * @return ErrorCode 
 */
ErrorCode Uart_get_char(uint8_t uart_id_number_in, char *recvd_byte_out);

/**
 * @brief Sends a byte from the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @param is_blocking_in bool to check if the function should be nonblocking.
 * @param byte_out Byte to send.
 * @return ErrorCode 
 */
ErrorCode Uart_put_char(uint8_t uart_id_number_in, char byte_out);

/**
 * @brief Sends a buffer with a specified length from the specified UART
 *  peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @param is_blocking_in bool to check if the function should be nonblocking.
 * @param buffer_length_in Length of buffer to send (in bytes).
 * @param buffer_out Pointer to the buffer/string to send.
 * @return ErrorCode 
 */
ErrorCode Uart_put_buffer(uint8_t uart_id_number_in, size_t buffer_length_in, char *buffer_out);

/**
 * @brief Checks the TX FIFO busy status of the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @return ErrorCode 
 */
ErrorCode Uart_busy_check_tx(uint8_t uart_id_number_in);

/**
 * @brief Checks the RX FIFO busy status of the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @return ErrorCode 
 */
ErrorCode Uart_busy_check_rx(uint8_t uart_id_number_in);

/* -------------------------------------------------------------------------   
 * INTERFACE 2.0
 * ------------------------------------------------------------------------- */

typedef uint8_t Uart_DeviceId;

#define UART_DEVICE_ID_GNSS (0)

/**
 * @brief Initialises the all used UART peripherals.
 * 
 * If not all UARTs are initialised correctly an error is returned for
 * monitoring purposes only. If all UARTs fail to initialise an error is also
 * returned, but indicates failure of the UART system itself.
 *
 * @return ErrorCode If no error, ERROR_NONE, otherwise UART_ERROR_x.
 */
// ErrorCode Uart_init(void);

/**
 * @brief Initialise (or re-init) a specific UART device.
 * 
 * @param uart_id_in The device to re-initialise.
 * @return ErrorCode TODO
 */
ErrorCode Uart_init_specific(Uart_DeviceId uart_id_in);

/**
 * @brief IF STATE MACHINE
 * 
 * @return ErrorCode 
 */
ErrorCode Uart_step(void);

/**
 * @brief Send bytes
 * 
 * Event EVT_UART_SEND_COMPLETE is raised when the bytes have been sent to the
 * device. This could either be successful or it could fail, so use
 * Uart_get_status() for the device to check.
 * 
 * @param uart_id_in 
 * @param p_data_in 
 * @param length_in 
 * @return ErrorCode 
 */
ErrorCode Uart_send_bytes(
    Uart_DeviceId uart_id_in,
    uint8_t *p_data_in, 
    size_t length_in
);

/**
 * @brief Did it work?
 * 
 * @param uart_id_in 
 * @param p_status_out 
 * @return ErrorCode 
 */
ErrorCode Uart_get_status(
    Uart_DeviceId uart_id_in,
    void *p_status_out
);


ErrorCode Uart_recv_bytes(
    Uart_DeviceId uart_id_in,
    uint8_t *p_data_out,
    size_t length_in
);

#endif /* H_UART_PUBLIC_H */