/**
 * Test name:       Test the functionality of watchdog modules and code
 * Last modified:   01/08/2019
 * Code Status:     Functional
 * Test result:     <result>/<date>
*/
#include "../firmware.h"
//#define USE_ONLY_EXTERNAL_WDT       //uncomment if you want to use only the external watchdog

int main(void)
{
  watchdog_update = 0xFF;
  Board_init();
  enable_watchdog_kick();
  #ifndef USE_ONLY_EXTERNAL_WDT
  setup_internal_watchdogs();
  #endif
  RTC_init();
  //some variables for the test use
  uint64_t time = 0;
  int count = 0;
  char output_buffer[100];

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nWATCHDOG FUNCTIONALITY TEST\r\nBoard, RTC and watchdog drivers initialised1\r\n");
  UART_puts(UART_INTERFACE, "\r\nFor the first part of the test \"watchdog_update\" variable will be frequently set to nominal value 0xFF");
  UART_puts(UART_INTERFACE, "\r\nWhen this is going, the watchdog kick for both internal and external watchdog will occur and the program will not reboot");
  RTC_getTime_ms(&time); sprintf(output_buffer, "\r\nTime at the start is:  %ul ms", time);
  UART_puts(UART_INTERFACE, output_buffer);
  while(1) {
    if(count<15){
      RTC_getTime_ms(&time);
      sprintf(output_buffer,"\r\n--->Time is: %ul ms ---> watchdog_update is now:  %d and will be set back to 0xFF", time, watchdog_update);
      UART_puts(UART_GNSS, output_buffer);
      watchdog_update = 0xFF;
      Delay_ms(10000);
      count ++;
    }
    else
    {
      RTC_getTime_ms(&time);
      sprintf(output_buffer,"\r\n--->Time is: %ul ms ---> watchdog_update is now: %d and we stopped setting it back to 0xFF ---> watchdog kick will not occur when this variable get 0", time, watchdog_update);
      UART_puts(UART_GNSS, output_buffer);
      Delay_ms(10000);
    }
  }
}