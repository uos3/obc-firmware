/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_GNSS

int main(void)
{
  uint32_t written1, read1, written2, read2;
  char output1[100];
  char output2[100];

  Board_init();

  LED_on(LED_B);

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nEEPROM Demo\r\n");

  EEPROM_init();

  if(EEPROM_selfTest() == false)
  {
    UART_puts(UART_INTERFACE, "\r\nSelftest: FAIL\r\n");
    LED_off(LED_B);
    while(1) {};
  }
  UART_puts(UART_INTERFACE, "\r\nSelftest: OK\r\n");

  while(1)
  {
    LED_on(LED_B);

    written1 = 1;
    EEPROM_write(0x0000, &written1, 4);

    sprintf(output1,"Written1: %d\r\n", written1);
    UART_puts(UART_INTERFACE, output1);

    EEPROM_read(0x0000, &read1, 4);

    sprintf(output1,"Read1: %d\r\n", read1);
    UART_puts(UART_INTERFACE, output1);

    written2 = 3;
    EEPROM_write(0x00000001, &written2, 4);

    sprintf(output2,"Written2: %d\r\n", written2);
    UART_puts(UART_INTERFACE, output2);

    EEPROM_read(0x00000001, &read2, 4);

    sprintf(output2,"Read2: %d\r\n\r\n", read2);
    UART_puts(UART_INTERFACE, output2);

    LED_off(LED_B);

    Delay_ms(5000);
  }
}
