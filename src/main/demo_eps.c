/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_CAMERA

int main(void){
  Board_init();
  EPS_init();

  LED_on(LED_B);

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nEPS Demo\r\n");

  char output[100];
  uint16_t v;

  EPS_getBatteryVoltage(&v);
  sprintf(output,"Voltage: %+06d\r\n", v);
  UART_puts(UART_INTERFACE, output);

  while(1){
    UART_puts(UART_INTERFACE, "\r\nHello World!\r\n");
    Delay_ms(500);
  }
}
