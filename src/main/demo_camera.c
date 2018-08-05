/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

uint32_t image_length;
void image_length_add(uint8_t *data, uint32_t data_length);

int main(void)
{
  char output[200];

  Board_init();
  RTC_init();

  UART_init(UART_GNSS, 115200);
  UART_puts(UART_GNSS, "Camera Demo\r\n");

  UART_init(UART_CAMERA, 115200);

  while(1)
  {
    LED_on(LED_B);
    UART_puts(UART_GNSS, "Capturing..\r\n");
    image_length = 0;

    if(Camera_capture(5000, image_length_add))
    {
        sprintf(output, "SUCCESS (%"PRIu32" Bytes)\r\n", image_length);
        UART_puts(UART_GNSS, output);
    }
    else
    {
        sprintf(output, "FAIL (%"PRIu32" Bytes)\r\n", image_length);
        UART_puts(UART_GNSS, output);
    }

    LED_off(LED_B);

    /* On period */
    Delay_ms(1000);
  }
}

void image_length_add(uint8_t *data, uint32_t data_length)
{
    char output[60];
    (void) data;
    
    image_length += data_length;

    sprintf(output, "0x%02"PRIx8", 0x%02"PRIx8" ..(%"PRIu32"B).. 0x%02"PRIx8", 0x%02"PRIx8"\r\n",
     data[0], data[1],
     data_length,
     data[data_length-2], data[data_length-1]
    );
    UART_puts(UART_GNSS, output);
}