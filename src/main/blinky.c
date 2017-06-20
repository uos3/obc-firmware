/* firmware.h contains all relevant headers */
#include "../firmware.h"

/* A very simple example that blinks the on-board LED. */

int main(void)
{
  volatile uint32_t i;

  Board_init();

  while(1)
  {
    LED_on(LED_B);

    /* On period */
    for(i = 0; i < 300000; i++) {};

    LED_off(LED_B);

    /* Off period */
    for(i = 0; i < 300000; i++) {};
  }
}
