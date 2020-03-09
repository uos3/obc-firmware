#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "../driver/uart.h"
#include "../driver/board.h"


void UART_init(uint8_t uart_num, uint32_t baudrate){}

char UART_getc(uint8_t uart_num){}

void UART_putc(uint8_t uart_num, char c){}

void UART_puts(uint8_t uart_num, char *str){
	if (uart_num == UART_DEBUG_4){
		printf(str);
	}
}

void UART_putb(uint8_t uart_num, char *str, uint32_t len){}

bool UART_getc_nonblocking(uint8_t uart_num, char *c){}

bool UART_putc_nonblocking(uint8_t uart_num, char c){}

bool UART_puts_nonblocking(uint8_t uart_num, char *str){}

bool UART_putb_nonblocking(uint8_t uart_num, char *str, uint32_t len){}

bool UART_busy(uint8_t uart_num){}

bool UART_charsAvail(uint8_t uart_num){}
