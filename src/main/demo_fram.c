/**
 * File purpose:        FRAM functionality demo
 * Last modification:   22/08/2019
 * Status:              Ready for test
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"
#include <stdio.h>

static uint32_t address;

#define DATA_BUFFER_LENGTH  50
static uint8_t data_write[DATA_BUFFER_LENGTH];
static uint8_t data_read[DATA_BUFFER_LENGTH];

int main(void)
{
  uint32_t i;
  char output[200];
  Board_init();
  watchdog_update = 0xFF;
  enable_watchdog_kick();

  UART_init(UART_INTERFACE, 9600);

  for(int i=0;i<3;i++)
  {LED_on(LED_B); Delay_ms(100); LED_off(LED_B);} //blink the LED three times

  UART_puts(UART_INTERFACE, "\r\n>>>>>>>> FRAM Memory Demo\r\n");

  while(1)
  {
    LED_on(LED_B);

    if(FRAM_selfTest())
    {
      UART_puts(UART_INTERFACE, "Self-test: PASSED\r\n");
    }
    else
    {
      UART_puts(UART_INTERFACE, "Self-test: FAILED\r\n");
    }

    /* Generate Random address */
    address = Random(FRAM_MAX_ADDRESS);

    /* Generate Random data */
    for(i=0;i<DATA_BUFFER_LENGTH;i++)
    {
      data_write[i] = (uint8_t)Random(UINT8_MAX-1);
      data_read[i] = 0x00;
    }

    sprintf(output,">>> Write:\r\n * Address: 0x%08lx\r\n * Data: 0x", address);
    for(i=0;i<DATA_BUFFER_LENGTH;i++)
    {
      sprintf(output, "%s%02x", output, data_write[i]);
    }
    sprintf(output, "%s\r\n", output);
    UART_puts(UART_INTERFACE, output);

    FRAM_write(address, data_write, DATA_BUFFER_LENGTH);

    FRAM_read(address, data_read, DATA_BUFFER_LENGTH);

    sprintf(output,">>> Read:\r\n * Address: 0x%08lx\r\n * Data: 0x", address);
    for(i=0;i<DATA_BUFFER_LENGTH;i++)
    {
      sprintf(output, "%s%02x", output, data_read[i]);
    }
    sprintf(output, "%s\r\n", output);
    UART_puts(UART_INTERFACE, output);

    if(memcmp(data_read, data_write, DATA_BUFFER_LENGTH) == 0)
    {
      UART_puts(UART_INTERFACE, ">>> Buffer Match: PASS\r\n");
    }
    else
    {
      UART_puts(UART_INTERFACE, ">>> Buffer Match: FAIL\r\n");
    }
    
    UART_puts(UART_INTERFACE, "---------------\r\n");
    LED_off(LED_B);

    watchdog_update = 0xFF;
    Delay_ms(20000);
  }
}
