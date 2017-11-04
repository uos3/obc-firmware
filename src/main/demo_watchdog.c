/* firmware.h contains all relevant headers */

/* A very simple example that blinks the on-board LED. */

// now modified to watchdog kick on interrupt

#include "../firmware.h"

int main(void)
{
  Board_init();

  WDT_kick();
  
  LED_off(LED_B);
  Delay_ms(1000); // so can be clearly seen when watchdog kicking

  setupwatchdoginterrupt();	// kick the watchdog on interrupt

  while(1) {}
}