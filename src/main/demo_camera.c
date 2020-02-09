/**
 * File purpose:        Current camera demo
 * Last modification:   27/07/2019
 * Status:              Functional
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  Board_init();
  watchdog_update = 0xFF;
  enable_watchdog_kick();
  char output[100];
  uint8_t page_buffer[108], slot_counter, data_output[BUFFER_SLOT_SIZE/8];
  //uint8_t holder_buffer[2000];
  uint64_t timestamp;
  uint32_t picture_size;
  uint16_t no_of_pages;
  RTC_init();
  Buffer_init();

  UART_init(UART_INTERFACE,9600);
  UART_puts(UART_INTERFACE, "\r\nCamera Demo\r\nSTART\r\n");

  RTC_getTime_ms(&timestamp);
  sprintf(output, "Start time is - %lu\r\n", timestamp);
  UART_puts(UART_INTERFACE ,output);

  UART_init(UART_CAMERA, 38400);
  watchdog_update = 0xFF;
  int no_of_pictures_to_take = 1;
  while(no_of_pictures_to_take--)
  {
    Delay_ms(5000);
    UART_puts(UART_INTERFACE, "Taking the picture...\r\n");
    //last argument determine if the picture data will be printed in the terminal
    // 0 - dont print data; 1 - print data
    if(!Camera_capture(&picture_size, &no_of_pages, 3, &slot_counter)){
      UART_puts(UART_INTERFACE, "Error: Picture not taken!\r\n");
    }
    else{
      //Address for camera data slots is from 3195 to 119427: slot_counter: 123; no of pages is 6; for image resolution profile 3
      //slot_counter is counting the number of times buffer_store_new_data is called, which should equal the number of slots of size 696
      //however, the slot counter is inaccurate. Through debug statements written in Buffer_FRAM_write_data, I have found out that the
      // actual number of slots is 167 if needed to read from FRAM.

      //Note: data that is being stored is always 536886332 for every slot, may indicate a problem with the camera or a wrong loop in the camera_capture function.
      sprintf(output, "Picture size taken : %d ; Number of Pages : %d, slot_counter: %d\r\n", picture_size, no_of_pages, slot_counter);
      UART_puts(UART_INTERFACE,"SUCCESS\r\n");
      UART_puts(UART_INTERFACE, output);
    }
    for(uint16_t counter = 0; counter<168; counter++){
      Buffer_FRAM_read_data(counter, &data_output);
      if(sizeof(data_output) == sizeof(uint8_t)){
        UART_puts(UART_INTERFACE, "Data collected\r\n");
        sprintf(output, "Data read is : %d at slot: %d\r\n", data_output, counter);
        UART_puts(UART_INTERFACE, output);
      } //else {
        //for(int data_counter = 0, data_counter<8; data_counter++){
        //  data_output[data_counter] = holder_buffer[counter];
        //}
      //}
      if(counter == 167 && data_output == 536886332){
        UART_puts(UART_INTERFACE, "Retrieval of data from FRAM success!");
      }
    }
    Delay_ms(10000);
  }
}
