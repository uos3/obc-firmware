/**
 * @defgroup uart
 *
 * UART Peripheral Driver.
 *
 * @details Hardware UART Peripheral Driver.
 *
 * UART ports for each board are defined in ${board}/board.h
 *
 * @ingroup drivers
 * @{
 */

#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * Initialises the specified UART peripheral at the given baudrate.
 *
 * @param uart_num ID of the UART peripheral
 * @param baudrate buadrate to be used on the UART peripheral
 */
void UART_init(uint8_t uart_num, uint32_t baudrate);

/**
 * Retrieves a byte from the RX FIFO of the specified UART peripheral,
 *  waits for a byte if RX FIFO is empty.
 *
 * @param uart_num ID of the UART peripheral
 * @returns byte received from UART peripheral
 */
char UART_getc(uint8_t uart_num);

/**
 * Sends a byte from the specified UART peripheral,
 *  waits to send if TX FIFO is full.
 *
 * @param uart_num ID of the UART peripheral
 * @param c byte to send
 */
void UART_putc(uint8_t uart_num, char c);

/**
 * Sends a null-terminated string from the specified UART peripheral,
 *  waits to send if TX FIFO is full.
 *
 * @param uart_num ID of the UART peripheral
 * @param str pointer to the string to send
 */
void UART_puts(uint8_t uart_num, char *str);

/**
 * Sends a buffer from the specified UART peripheral,
 *  waits to send if TX FIFO is full.
 *
 * @param uart_num ID of the UART peripheral
 * @param str pointer to the string to send
 * @param len number of bytes in the buffer to send
 */
void UART_putb(uint8_t uart_num, char *str, uint32_t len);


/**
 * Retrieves a byte from the RX FIFO of the specified UART peripheral,
 *  returns if RX FIFO is empty.
 *
 * @param uart_num ID of the UART peripheral
 * @param c pointer to a variable for the received byte
 * @returns true if a byte was retrieved, false otherwise
 */
bool UART_getc_nonblocking(uint8_t uart_num, char *c);

/**
 * Sends a byte from the specified UART peripheral,,
 *  returns if TX FIFO is full.
 *
 * @param uart_num ID of the UART peripheral
 * @param c byte to send
 * @returns true if the byte was sent, false otherwise
 */
bool UART_putc_nonblocking(uint8_t uart_num, char c);

/**
 * Sends a null-terminated string from the specified UART peripheral,
 *  returns if TX FIFO is full.
 *
 * @param uart_num ID of the UART peripheral
 * @param str pointer to the string to send
 * @returns true if the string was sent, false otherwise
 */
bool UART_puts_nonblocking(uint8_t uart_num, char *str);

/**
 * Sends a buffer from the specified UART peripheral,
 *  returns if TX FIFO is full.
 *
 * @param uart_num ID of the UART peripheral
 * @param str pointer to the string to send
 * @param len number of bytes in the buffer to send
 * @returns true if the string was sent, false otherwise
 */
bool UART_putb_nonblocking(uint8_t uart_num, char *str, uint32_t len);

/**
 * Retrieves the TX FIFO status of the specified UART peripheral.
 *
 * @param uart_num ID of the UART peripheral
 * @returns true if there are bytes waiting in the TX FIFO, false otherwise
 */
bool UART_busy(uint8_t uart_num);

/**
 * Retrieves the RX FIFO status of the specified UART peripheral.
 *
 * @param uart_num ID of the UART peripheral
 * @returns true if there are bytes waiting in the RX FIFO, false otherwise
 */
bool UART_charsAvail(uint8_t uart_num);

#endif /*  __UART_H__ */

/**
 * @}
 */