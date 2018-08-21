/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_GNSS

int main(void)
{
  int16_t x, y, z;
  char output[200];

  Board_init();
  setupwatchdoginterrupt();

  LED_on(LED_B);

  UART_init(UART_INTERFACE, 9600);

  IMU_Init();

  if(IMU_selftest() == false)
  {
    UART_puts(UART_INTERFACE, "\r\nSelftest: FAIL\r\n");
    LED_off(LED_B);
    while(1) {};
  }

  while(1)
  {
    LED_on(LED_B);

    IMU_read_accel(&x, &y, &z);
    sprintf(output,"%+06d,%+06d,%+06d", x, y, z);

    IMU_read_gyro(&x, &y, &z);
    sprintf(output,"%s,%+06d,%+06d,%+06d", output, x, y, z);

    IMU_read_magno(&x, &y, &z);
    sprintf(output,"%s,%+06d,%+06d,%+06d", output, x, y, z);

    IMU_read_temp(&x);
    sprintf(output,"%s,%+06d\r\n", output, x);
    LED_off(LED_B);

    UART_puts(UART_INTERFACE, output);

    Delay_ms(100);
  }
}
