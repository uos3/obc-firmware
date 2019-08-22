/**
 * File purpose:        RTC functionality demo
 * Last modification:   22/08/2019
 * Status:              Ready for the test
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

int main(void)
{
  uint32_t clock;
  uint64_t clock_ms;
  uint32_t setclock = 1000000;
  char output[100];

  Board_init();
  watchdog_update = 0xFF;
  enable_watchdog_kick();
  UART_init(UART_INTERFACE, 9600);
  LED_on(LED_B);

  UART_puts(UART_INTERFACE, "\r\nRTC Demo\r\n");
  RTC_init();

  uint8_t i=0;
  while(1)
  {
    LED_on(LED_B);
    /* For first ten loops just read and print the time, for 10th loop test manual time setting */
    if(i<10)
    {
      i++;
    }
    else if(i==10)
    {
      i = 20;
      /* When i == 10 set the time Manually to the value of setclock */
      sprintf(output,"Manually setting clock to: %+011ld\r\n", setclock);
      UART_puts(UART_INTERFACE, output);

      RTC_setTime(&setclock);
    }
    /* Check the time values */
    RTC_getTime(&clock);
    RTC_getTime_ms(&clock_ms);

    sprintf(output,"Time     : %011"PRIu32"\r\n", clock);
    UART_puts(UART_INTERFACE, output);
    sprintf(output,"Time (ms): %011"PRIu64"\r\n", clock_ms);
    UART_puts(UART_INTERFACE, output);

    LED_off(LED_B);
    watchdog_update = 0xFF;
    Delay_ms(1000);
  }
}
