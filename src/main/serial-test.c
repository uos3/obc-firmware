/* firmware.h contains all relevant headers */
#include "../firmware.h"

int main(void)
{
  Board_init();
  WDT_kick();

  LED_on(LED_B);

  UART_init(UART_CAM_HEADER, 9600);
  UART_init(UART_PC104_HEADER, 9600);

  while(1)
  {
    LED_off(LED_B);

    UART_putc(UART_CAM_HEADER, 'U');
    UART_putc(UART_PC104_HEADER, 'U');

    /* Off period */
    Delay_ms(750);
    WDT_kick();

    LED_on(LED_B);

    UART_puts(UART_CAM_HEADER, "\r\nCamera Header\r\n");
    UART_puts(UART_PC104_HEADER, "\r\nPC104 Header\r\n");

    /* On period */
    Delay_ms(250);
    WDT_kick();
  }
}
