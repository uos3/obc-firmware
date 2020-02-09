/**
 * File purpose:        Simple LED blinky demo 
 * Last modification:   22/08/2019
 * Status:              Functional
 */
#include "../firmware.h"

int main(void)
{
  Board_init();
  watchdog_update = 0xFF;
  enable_watchdog_kick();

  while(1)
  {
    LED_on(LED_B);

    /* On period */
    Delay_ms(5000);
    WDT_kick();

    LED_off(LED_B);

    /* Off period */
    watchdog_update = 0xFF;
    Delay_ms(5000);
    WDT_kick();
  }
}