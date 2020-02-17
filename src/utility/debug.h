/*
    Debug tools, feel free to expand.

    Author: Richard A
    Last modified: 2020 02 13
    Status: Functional
*/


#ifndef __DEBUG_H__
#define __DEBUG_H__

// toggle comment when not debugging
#define DEBUG_MODE

#include "../driver/board.h"

// protection against not having it defined
#ifdef DEBUG_MODE
    #define UART_INTERFACE UART_DEBUG_4
#endif

void debug_init();

void debug_print(char* debug_message);

void debug_clear();

#endif /*  __DEBUG_H__ */
