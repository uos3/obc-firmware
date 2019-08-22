/**
 * File purpose:        EEPROM demo
 * Last modification:   22/08/2019
 * Status:              Ready to test
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"
#include <stdio.h>

int main(void)
{
  uint32_t written1, read1, written23, read23;
  char output1[100];
  char output2[100];
  Board_init();                     //init board clock
  watchdog_update = 0xFF;           //init watchdog_update variable
  enable_watchdog_kick();           //enable external watchdog kick
  UART_init(UART_INTERFACE, 9600);  //init UART

  for(int i=0;i<3;i++)
  {LED_on(LED_B); Delay_ms(100); LED_off(LED_B);} //blink the LED three times

  UART_puts(UART_INTERFACE, "\r\n>>>>>>>> EEPROM Demo\r\n");
  /* Initialise EEPROM */
  if(!EEPROM_init()){
    UART_puts(UART_INTERFACE, ">>> EEPROM initialisation failed!\r\n");
    while(1) {};
  }
  UART_puts(UART_INTERFACE, ">>> EEPROM initialised!\r\n");
  /* Perform EEPROM Selftest */
  if(!EEPROM_selfTest())
  {
    UART_puts(UART_INTERFACE, "\r\nSelftest: FAIL\r\n");
    while(1) {};
  }
  UART_puts(UART_INTERFACE, "\r\nSelftest: OK\r\n");

  while(1)
  {
    LED_on(LED_B);
    /* Write a number to EEPROM */
    written1 = 1;
    EEPROM_write(0x0000, &written1, 4);

    sprintf(output1,">>> Written1: %d\r\n", written1);
    UART_puts(UART_INTERFACE, output1);
    /* Read it */
    EEPROM_read(0x0000, &read1, 4);

    sprintf(output1,">>> Read1: %d\r\n", read1);
    UART_puts(UART_INTERFACE, output1);
    /* Write another number to EEPROM */
    written23 = 23;
    EEPROM_write(0x00000001, &written2, 4);

    sprintf(output2,">>> Written23: %d\r\n", written2);
    UART_puts(UART_INTERFACE, output2);
    /* And Read it */
    EEPROM_read(0x00000001, &read2, 4);

    sprintf(output2,">>> Read23: %d\r\n\r\n", read2);
    UART_puts(UART_INTERFACE, output2);

    LED_off(LED_B);

    watchdog_update = 0xFF;
    Delay_ms(5000);
  }
}
