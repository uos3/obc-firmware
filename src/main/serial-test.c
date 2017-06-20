/* firmware.h contains all relevant headers */
#include "../firmware.h"

int main(void)
{
  volatile uint32_t i;

  Board_init();

  LED_on(LED_B);

  UART_init(UART_CAMERA, 9600);
  UART_init(UART_GPS, 9600);

  while(1)
  {
    LED_off(LED_B);

    /* Off period */
    for(i = 0; i < 300000; i++) {};

    LED_on(LED_B);

    UART_putc(UART_CAMERA, 't');
    UART_putc(UART_GPS, 't');

    UART_puts(UART_CAMERA, "Test\n");
    UART_puts(UART_GPS, "Test\n");

    /* On period */
    for(i = 0; i < 100000; i++) {};
  }
}
