/* firmware.h contains all relevant headers */
/*#include "../firmware.h"
#include <stdio.h>

uint32_t image_length;
uint8_t *image_data;
void image_length_add(uint8_t *data, uint32_t data_length);
uint8_t take_picture();


uint8_t take_picture(){

    char output[200];

    RTC_init();
    UART_init(UART_CAMERA, 115200);

    LED_on(LED_B);
    image_length = 0;

    if(Camera_capture(5000, image_length_add))
    {
        return 0;
    }
    else
    {
        return 1;
    }
    LED_off(LED_B);
  }


void image_length_add(uint8_t *data, uint32_t data_length){
    (void) data;
    image_data = data;
    
    image_length += data_length;


}*/

#include "../firmware.h"

#include <stdio.h>

uint32_t image_length;
void image_length_add(uint8_t *data, uint32_t data_length);

int8_t get_image_data(void)
{
  char output[200];

  Board_init();
  RTC_init();

  UART_init(UART_INTERFACE, 9600);

  UART_init(UART_CAMERA, 115200);


    LED_on(LED_B);
    UART_puts(UART_INTERFACE, "Capturing image...\r\n");
    image_length = 0;

    if(Camera_capture(5000, image_length_add))
    {
        sprintf(output, "SUCCESS (%"PRIu32" Bytes)\r\n", image_length);
        UART_puts(UART_INTERFACE, output);
    }
    else
    {
        sprintf(output, "FAIL (%"PRIu32" Bytes)\r\n", image_length);
        UART_puts(UART_INTERFACE, output);
        return 1;
    }
    UART_puts(UART_INTERFACE, "Finished capturing image.\r\n")
    LED_off(LED_B);

    /* On period */

    Delay_ms(1000);
    return 0;
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
    UART_puts(UART_INTERFACE, output);
}