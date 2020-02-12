#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "../driver/board.h"

#define DEBUG_A GPIO_PC4
#define DEBUG_B GPIO_PD4
#define DEBUG_C GPIO_PD5
#define DEBUG_D GPIO_PB0
#define DEBUG_E GPIO_PC5

/* Uncomment when blackmagic connected to UART 4. Will allow for debug prints. */
// #define DEBUG_MODE

/* UART_INTEFRACE is commonly used to indicate the debug line. WIll raise an
error if not defined. It will only be defined if debug mode is active, which
is the only circumstance the print statements should be used. */
#ifdef DEBUG_MODE
    #define UART_INTERFACE UART_DEBUG_4
#endif

void Debug_print(char* fmt, ...);

#endif /*  __DEBUG_H__ */
