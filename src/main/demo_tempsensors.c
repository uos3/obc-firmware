/**
 * File purpose:        Temperature sensors functionality Demo
 * Last modification:   22/08/2019
 * Status:              Ready for the test
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"
#include <stdio.h>

int main(void)
{
  char output[100];
  Board_init();                 //initialise board clock
  watchdog_update = 0xFF;       //initialise watchdog_update variable
  enable_watchdog_kick();       //initialise watchdog kick functionality
  UART_init(UART_INTERFACE, 9600);

  for(int i=0;i<3;i++)
  {LED_on(LED_B); Delay_ms(100); LED_off(LED_B);} //blink the LED three times

  UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Temperature Sensors Demo\r\n");
  UART_puts(UART_INTERFACE, ">>>Lets read the Temperature from the sensors on board\r\n\r\n");
  
  while(1)
  {
    LED_on(LED_B);  //LED_B will be on while measuring temperature
    sprintf(output,">>> PCT2075: %+.1f°C, TMP100: %+.1f°C\r\n",
      ((double)Temperature_read_pct2075()/10),
      ((double)Temperature_read_tmp100()/10)
      );
    UART_puts(UART_INTERFACE, output);
    LED_off(LED_B); 
    Delay_ms(5000); //add 5s delay for easiest result observations
    watchdog_update = 0xFF; //keep updating the watchdog_update variable to prevent watchdog reset
  }
}
