/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_CAMERA

int main(void)
{
  char output[100];

  Board_init();

  LED_on(LED_B);

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nTemperature Sensors Demo\r\n");

  while(1)
  {
    LED_on(LED_B);

    sprintf(output,"PCT2075: %+.1f°C, TMP100: %+.1f°C\r\n",
      ((double)Temperature_read_pct2075()/10),
      ((double)Temperature_read_tmp100()/10)
      );
    UART_puts(UART_INTERFACE, output);

    LED_off(LED_B);

    Delay_ms(200);
  }
}
