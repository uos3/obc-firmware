/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_GNSS

static uint32_t address;

#define DATA_BUFFER_LENGTH  50
static uint8_t data_write[DATA_BUFFER_LENGTH];
static uint8_t data_read[DATA_BUFFER_LENGTH];

int main(void)
{
  uint32_t i;
  char output[200];

  Board_init();

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nFRAM Memory Demo\r\n");

  while(1)
  {
    LED_on(LED_B);

    if(FRAM_selfTest())
    {
      UART_puts(UART_INTERFACE, "Self-test: PASS\r\n");
    }
    else
    {
      UART_puts(UART_INTERFACE, "Self-test: FAIL\r\n");
    }

    /* Generate Random address */
    address = Random(FRAM_MAX_ADDRESS);

    /* Generate Random data */
    for(i=0;i<DATA_BUFFER_LENGTH;i++)
    {
      data_write[i] = (uint8_t)Random(UINT8_MAX-1);
      data_read[i] = 0x00;
    }

    sprintf(output,"Write:\r\n * Address: 0x%08lx\r\n * Data: 0x", address);
    for(i=0;i<DATA_BUFFER_LENGTH;i++)
    {
      sprintf(output, "%s%02x", output, data_write[i]);
    }
    sprintf(output, "%s\r\n", output);
    UART_puts(UART_INTERFACE, output);

    FRAM_write(address, data_write, DATA_BUFFER_LENGTH);

    FRAM_read(address, data_read, DATA_BUFFER_LENGTH);

    sprintf(output,"Read:\r\n * Address: 0x%08lx\r\n * Data: 0x", address);
    for(i=0;i<DATA_BUFFER_LENGTH;i++)
    {
      sprintf(output, "%s%02x", output, data_read[i]);
    }
    sprintf(output, "%s\r\n", output);
    UART_puts(UART_INTERFACE, output);

    if(memcmp(data_read, data_write, DATA_BUFFER_LENGTH) == 0)
    {
      UART_puts(UART_INTERFACE, "Buffer Match: PASS\r\n");
    }
    else
    {
      UART_puts(UART_INTERFACE, "Buffer Match: FAIL\r\n");
    }
    
    UART_puts(UART_INTERFACE, "---------------\r\n");
    LED_off(LED_B);
    
    Delay_ms(5000);
  }
}
