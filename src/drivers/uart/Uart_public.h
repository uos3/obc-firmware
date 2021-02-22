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
ErrorCode Uart_get_char(uint8_t uart_id_number_in, bool is_nonblocking_in, char *recvd_byte_out);

/**
 * @brief Sends a byte from the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @param is_blocking_in bool to check if the function should be nonblocking.
 * @param byte_out Byte to send.
 * @return ErrorCode 
 */
ErrorCode Uart_put_char(uint8_t uart_id_number_in, bool is_nonblocking_in, char byte_out);

/**
 * @brief Sends a string (/0 terminated) from the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @param is_blocking_in bool to check if the function should be nonblocking.
 * @param string_out Pointer to the string to send.
 * @return ErrorCode 
 */
ErrorCode Uart_put_string(uint8_t uart_id_number_in, bool is_nonblocking_in, char *string_out);

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
ErrorCode Uart_put_buffer(uint8_t uart_id_number_in, bool is_nonblocking_in, size_t buffer_length_in, char *buffer_out);

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

#endif /* H_UART_PUBLIC_H */