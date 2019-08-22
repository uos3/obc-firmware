/**
 * File purpose:        FRAM functionality demo v2
 * Last modification:   22/08/2019
 * Status:              Not known - the only difference is using snprintf instea of for loop
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define UART_INTERFACE UART_CAMERA

static uint32_t address;

#define DATA_BUFFER_LENGTH  50
static uint8_t data_write[DATA_BUFFER_LENGTH];
static uint8_t data_read[DATA_BUFFER_LENGTH];

#define PRINT_BUFFER_LENGTH 200
uint32_t print_buffer_counter;

#define PRINTF_BUFFER_COUNTER_APPEND_ARGS(output, print_buffer_counter, ...) \
  print_buffer_counter += (size_t)snprintf( \
    (output+print_buffer_counter), \
    (PRINT_BUFFER_LENGTH-print_buffer_counter), \
    __VA_ARGS__ \
  );

int main(void)
{
  uint32_t i;
  char output[PRINT_BUFFER_LENGTH];
  watchdog_update = 0xFF;
  Board_init();
  enable_watchdog_kick();

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

    print_buffer_counter = 0;
    PRINTF_BUFFER_COUNTER_APPEND_ARGS(
      output,
      print_buffer_counter,
      "Write:\r\n * Address: 0x%08lx\r\n * Data: 0x",
      address
    );
    for(i=0;i<DATA_BUFFER_LENGTH;i++)
    {
      PRINTF_BUFFER_COUNTER_APPEND_ARGS(
        output,
        print_buffer_counter,
        "%02x",
        data_write[i]
      );
    }
    PRINTF_BUFFER_COUNTER_APPEND_ARGS(
      output,
      print_buffer_counter,
      "\r\n"
    );
    UART_puts(UART_INTERFACE, output);

    FRAM_write(address, data_write, DATA_BUFFER_LENGTH);

    FRAM_read(address, data_read, DATA_BUFFER_LENGTH);

    print_buffer_counter = 0;
    PRINTF_BUFFER_COUNTER_APPEND_ARGS(
      output,
      print_buffer_counter,
      "Read:\r\n * Address: 0x%08lx\r\n * Data: 0x",
      address
    );
    for(i=0;i<DATA_BUFFER_LENGTH;i++)
    {
      PRINTF_BUFFER_COUNTER_APPEND_ARGS(
        output,
        print_buffer_counter,
        "%02x",
        data_read[i]
      );
    }
    PRINTF_BUFFER_COUNTER_APPEND_ARGS(
      output,
      print_buffer_counter,
      "\r\n"
    );
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
    watchdog_update = 0xFF;
    Delay_ms(5000);
  }
}
