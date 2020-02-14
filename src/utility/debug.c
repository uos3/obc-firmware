/*
    Debug tools, feel free to expand.

    Author: Richard A
    Last modified: 2020 02 13
    Status: Functional
*/

#include "../driver/board.h"

#include "../driver/uart.h"
#include "../component/led.h"

#include "debug.h"


void debug_init(){
  UART_init(UART_INTERFACE, 9600);
  LED_on(LED_B);
}

void debug_print(char* debug_message) {
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, debug_message); 
    UART_puts(UART_INTERFACE, "\r\n");
  #endif
}

