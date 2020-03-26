/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_GNSS

uint64_t last_WDT;

int main(void){

  char output[100];
  UART_init(UART_INTERFACE, 9600);
  Board_init();
  RTC_getTime_ms(&last_WDT);

  UART_puts(UART_INTERFACE, "Watchdog Demo Begin\n\r");
  
  while(1){
    UART_puts(UART_INTERFACE, "Before Kicked\n\r");
    WDT_wait_to_kick(&last_WDT);
    UART_puts(UART_INTERFACE, "Watchdog Kicked\n\r");
    WDT_wait_to_kick(&last_WDT);


  }
}
