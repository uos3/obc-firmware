/* firmware.h contains all relevant headers
*  
*  For the purpose of this demo, add printing statements
*  to the interrupt routines of the watchdog to determine
*  if they are correctly called
*/
#include "../firmware.h"

int main(void)
{
  uint32_t time;
  char output[100];
  Board_init();
  RTC_init();
  UART_init(UART_GNSS, 9600);
  update_watchdog_timestamp();
  sprintf(output, "Mission loop time is %u \r\n", missionloop_time);
  UART_puts(UART_GNSS, output);
  enable_watchdog_kick();
  setup_internal_watchdogs();
  UART_puts(UART_GNSS, "Start\r\n");
  LED_off(LED_B);
  int count = 0;

  while(1) {
    if(count<15){
    update_watchdog_timestamp();
    sprintf(output, "Mission loop time is %u \r\n", missionloop_time);
    UART_puts(UART_GNSS, output);
    Delay_ms(10000);
    count ++;
    }
    else
    {
      RTC_getTime(&time);
      sprintf(output, "time is %u \r\n", time);
      UART_puts(UART_GNSS, output);
      Delay_ms(10000);
    }
    
  }
}