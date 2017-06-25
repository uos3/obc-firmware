/* firmware.h contains all relevant headers */
#include "../firmware.h"

/* A very simple example that blinks the on-board LED. */

int main(void)
{
  Board_init();
  WDT_kick();

  while(1)
  {
    LED_on(LED_B);

    /* On period */
    Delay_ms(500);
    WDT_kick();

    LED_off(LED_B);

    /* Off period */
    Delay_ms(500);
    WDT_kick();
  }
}