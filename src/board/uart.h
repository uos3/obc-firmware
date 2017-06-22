#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>
#include <stdbool.h>

void UART_init(uint8_t uart_num, uint32_t baudrate);
char UART_getc(uint8_t uart_num);
void UART_putc(uint8_t uart_num, char c);
void UART_puts(uint8_t uart_num, char *str);
void UART_putb(uint8_t uart_num, char *str, uint32_t len);
bool UART_busy(uint8_t uart_num);
bool UART_charsAvail(uint8_t uart_num);

#endif /*  __UART_H__ */