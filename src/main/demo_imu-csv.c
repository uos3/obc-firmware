/**
 * File purpose:        IMU functionality demo
 * Last modification:   22/08/2019
 * Status:              Ready for the test
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"
#include <stdio.h>

int main(void)
{
  int16_t x, y, z;
  char output[200];

  Board_init();
  watchdog_update = 0xFF;
  enable_watchdog_kick();
  UART_init(UART_INTERFACE, 9600);

  for(int i=0;i<3;i++)
  {LED_on(LED_B); Delay_ms(100); LED_off(LED_B);} //blink the LED three times

  IMU_Init();

  if(!IMU_selftest())
  {
    UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Selftest: FAIL\r\n");
    while(1) {};
  }
  UART_puts(UART_INTERFACE, ">>> Taking the measurements\r\n");
  
  while(1)
  {
    LED_on(LED_B);

    IMU_read_gyro(&x, &y, &z);
    sprintf(output,"%s,%+06d,%+06d,%+06d", output, x, y, z);

    IMU_read_magno(&x, &y, &z);
    sprintf(output,"%s,%+06d,%+06d,%+06d\r\n", output, x, y, z);
    UART_puts(UART_INTERFACE, output);

    LED_off(LED_B);

    watchdog_update = 0xFF;
    Delay_ms(20000);
  }
}
