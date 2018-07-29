/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_CAMERA

int main(void)
{
  uint32_t clock;
  uint64_t clock_ms;
  uint32_t setclock = 1000000;
  char output[100];

  Board_init();

  LED_on(LED_B);

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nRTC Demo\r\n");

  RTC_init();

  uint8_t i=0;
  while(1)
  {
    LED_on(LED_B);

    if(i<10)
    {
      i++;
    }
    else if(i==10)
    {
      i = 20;
      
      sprintf(output,"Manually setting clock to: %+011ld\r\n", setclock);
      UART_puts(UART_INTERFACE, output);

      RTC_setTime(&setclock);
    }

    RTC_getTime(&clock);
    RTC_getTime_ms(&clock_ms);

    sprintf(output,"Time     : %011"PRIu32"\r\n", clock);
    UART_puts(UART_INTERFACE, output);
    sprintf(output,"Time (ms): %011"PRIu64"\r\n", clock_ms);
    UART_puts(UART_INTERFACE, output);

    LED_off(LED_B);

    Delay_ms(1000);
  }
}
