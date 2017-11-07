/* firmware.h contains all relevant headers */
#include "../firmware.h"

int main(void)
{
  Board_init();
  WDT_kick();

  LED_on(LED_B);

  UART_init(UART_CAMERA, 9600);
  UART_init(UART_GNSS, 9600);

  while(1)
  {
    LED_off(LED_B);

    UART_putc(UART_CAMERA, 'U');
    UART_putc(UART_GNSS, 'U');

    /* Off period */
    Delay_ms(750);
    WDT_kick();

    LED_on(LED_B);

    UART_puts(UART_CAMERA, "\r\nCamera Header\r\n");
    UART_puts(UART_GNSS, "\r\nPC104 Header\r\n");

    /* On period */
    Delay_ms(250);
    WDT_kick();
  }
}
