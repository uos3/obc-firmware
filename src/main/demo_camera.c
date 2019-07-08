/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>
#include <stdlib.h>

//#define PAGE_LENGTH 5000 //Buffer_slot_size is 3392bytes, do changed the page size!
#define PAGE_LENGTH 3392

uint32_t image_length, number_of_pages;     //variables to store the length of the received image and number of pages received
uint8_t *total_data;                        //variable to store the picture

int main(void)
{
  int dupa = 454;
  char output[100];
  char time[100];
  uint64_t timestamp;
  Board_init();
  RTC_init();

  UART_init(UART_GNSS, 115200);
  UART_puts(UART_GNSS, "Camera Demo\r\n");
  UART_init(UART_CAMERA, 115200);
  int stop = 1;
  while(1)
  { 
    image_length = 0;
    number_of_pages = 0;
    LED_on(LED_B);
    //total_data = (uint8_t) malloc(sizeof(uint8_t) * 50 * PAGE_LENGTH);
    //total_data = (uint8_t) malloc(sizeof(uint8_t) * 2 * PAGE_LENGTH);
    UART_puts(UART_GNSS, "\r\nCapturing..\r\n");
    RTC_getTime_ms(&timestamp);
    sprintf(time, "Time is %d\r\n",timestamp);
    UART_puts(UART_GNSS, time);

    if(Camera_capture(PAGE_LENGTH, &image_length, &number_of_pages, total_data))
    {
        sprintf(output, "Number of received pages is %d; image length is %d \r\n", number_of_pages, image_length);
        UART_puts(UART_GNSS,output);
        char output[20];
        int print = 0;
        while (print<=image_length))
        {
          sprintf(output, "0x%02"PRIx8" ", total_data[print]);
          UART_puts(UART_GNSS, output);
          print++;
        }
    }
    else
    {
        sprintf(output, "FAIL (%"PRIu32" Bytes) -- (Number of received pages - %"PRIu32")\r\n", image_length, number_of_pages);
        UART_puts(UART_GNSS, output);
    }
    LED_off(LED_B);
    /* On period */
    Delay_ms(1000);
    //free(total_data);
  }
}
/* 
void image_length_add(uint8_t *data, uint32_t data_length, uint32_t endfoundcount)
{
    char output[60];
    //char output2[60];
    //test
    //DOES NOT WORK, CAUSES DRIVER TO FAIL
    //char data_out[data_length];
    (void) data;
    //sprintf(output2, "endfoundcount is: %d\r\n", endfoundcount);
    //UART_puts(UART_GNSS, output2);
    image_length += data_length;
    //if (endfoundcount == 0){
    //test
    /*for(uint32_t k=0; k<data_length; k++){
        total_data[next_frame] = data[k];
        //UART_puts(UART_GNSS, "data\r\n");
        //sprintf(data_out,"%02" PRIx8 ", ", data[k] );
        //UART_puts(UART_GNSS, data_out);
        next_frame++;
    //}
    }
    total_data[endfoundcount] = *data;  //endfoundcount is the value of counter and show the index of the data line/frame/page received
    sprintf(output, "0x%02"PRIx8", 0x%02"PRIx8"  PRIu32"B).. 0x%02"PRIx8", "0x%02"PRIx8"\r\n",
     data[0], data[1],
     data_length,
     data[data_length-2], data[data_length-1]
    );
    UART_puts(UART_GNSS, output);
}*/