/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_CAM_HEADER

int main(void)
{
  int16_t x, y, z;
  char output[100];

  Board_init();

  LED_on(LED_B);

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nIMU Demo\r\n");

  IMU_Init();

  if(IMU_selftest() == false)
  {
    UART_puts(UART_INTERFACE, "\r\nSelftest: FAIL\r\n");
    LED_off(LED_B);
    while(1) {};
  }
  UART_puts(UART_INTERFACE, "\r\nSelftest: OK\r\n");

  while(1)
  {
    LED_on(LED_B);

    IMU_read_accel(&x, &y, &z);
    sprintf(output,"Accel: X %+06d, Y %+06d, Z %+06d\r\n", x, y, z);
    UART_puts(UART_INTERFACE, output);

    IMU_read_gyro(&x, &y, &z);
    sprintf(output,"Gyro:  X %+06d, Y %+06d, Z %+06d\r\n", x, y, z);
    UART_puts(UART_INTERFACE, output);

    IMU_read_magno(&x, &y, &z);
    sprintf(output,"Magno: X %+06d, Y %+06d, Z %+06d\r\n", x, y, z);
    UART_puts(UART_INTERFACE, output);

    UART_puts(UART_INTERFACE, "--------------------------------\r\n");

    LED_off(LED_B);

    Delay_ms(200);
  }
}
