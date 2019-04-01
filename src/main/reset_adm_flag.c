/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_GNSS

int main(void)
{
  uint32_t written, read;
  char output[100];

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

    written = 0;
    EEPROM_write(EEPROM_MISSION_DATA_ADDRESS, &written, 4);

    sprintf(output,"Written: %d\r\n", written);
    UART_puts(UART_INTERFACE, output);

    EEPROM_read(EEPROM_MISSION_DATA_ADDRESS, &read, 4);

    sprintf(output,"Read: %d\r\n", read);
    UART_puts(UART_INTERFACE, output);

    /*EEPROM_read(0x00000000, &read, 4);

    sprintf(output,"Read: 0x%08lx\r\n", read);
    UART_puts(UART_INTERFACE, output);*/


    LED_off(LED_B);

    Delay_ms(5000);
  }
}
