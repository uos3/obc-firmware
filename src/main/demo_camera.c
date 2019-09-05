/**
 * File purpose:        Current camera demo
 * Last modification:   27/07/2019
 * Status:              Functional
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  Board_init();
  watchdog_update = 0xFF;
  enable_watchdog_kick();
  char output[30];
  uint64_t timestamp;
  uint32_t picture_size;
  uint16_t no_of_pages;
  RTC_init();
  
  UART_init(UART_INTERFACE,115200);
  UART_puts(UART_INTERFACE, "\r\nCamera Demo\r\nSTART\r\n");

  RTC_getTime_ms(&timestamp);
  sprintf(output, "Start time is - %lu\r\n", timestamp);
  UART_puts(UART_INTERFACE ,output);

  UART_init(UART_CAMERA, 38400);
  watchdog_update = 0xFF;
  int no_of_pictures_to_take = 1;
  while(no_of_pictures_to_take--)
  {
    Delay_ms(5000);
    UART_puts(UART_INTERFACE, "Taking the picture...\r\n");
    //last argument determine if the picture data will be printed in the terminal
    // 0 - dont print data; 1 - print data
    if(!Camera_capture(&picture_size, &no_of_pages, 3)){
      UART_puts(UART_INTERFACE, "Error: Picture not taken!\r\n");
    }
    else{
      UART_puts(UART_INTERFACE,"SUCCES\r\n");
    }
  }
}