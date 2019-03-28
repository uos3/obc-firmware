/** @file blinky.c
 *
 * LED Blink Example Application
 *
 * @ingroup mains
 * @{
 */
#include "../firmware.h"

int main(void)
{
  Board_init();
  WDT_kick();

  while(1)
  {
    LED_on(LED_B);

    /* On period */
    Delay_ms(5000);
    WDT_kick();

    LED_off(LED_B);

    /* Off period */
    Delay_ms(5000);
    WDT_kick();
  }
}

/**
 * @}
 */
