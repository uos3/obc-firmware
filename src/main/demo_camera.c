/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>
#include <stdlib.h>

//#define PAGE_LENGTH 5000 //Buffer_slot_size is 3392bytes, do changed the page size!
#define PAGE_LENGTH 106

//uint32_t image_length, number_of_pages;     //variables to store the length of the received image and number of pages received
//uint8_t *total_data;                        //variable to store the picture
static char LK_RESET[]     = {0x56, 0x00, 0x26, 0x00};
static char LK_RESET_RE[]    = {0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};
static char LK_RESOLUTION_1600[] = {0x56, 0x00, 0x54, 0x01, 0x21};
static char LK_RESOLUTION_RE[] = {0x76, 0x00, 0x54, 0x00, 0x00}; // v T; response from the camera after setting the resolution

int main(void)
{
  /*char output[100];
  /char time[100];
  uint64_t timestamp;*/
  Board_init();
  RTC_init();

  UART_init(UART_GNSS,115200);
  UART_puts(UART_GNSS, "Camera Demo\r\n");
  UART_init(UART_CAMERA, 38400);
  int stop = 5;
  while(stop--)
  { 
    //image_length = 0;
    //number_of_pages = 0;
    /*LED_on(LED_B);
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
        while (print<=(image_length-1))
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
    stop--;*/
    UART_puts(UART_GNSS, "Before the for loop");
    uint8_t data[20] = {0};
    uint8_t data2[20] = {1};
    char output[20];
    char* x;
    UART_putb(UART_CAMERA, LK_RESET, sizeof(LK_RESET));
    Delay_ms(25);
    int index = 0;
    int j = 0, z =0;
    while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&data[j]);
      if(data[j] == LK_RESET_RE[index]){
        index++;
      }else{
        index = 0;
      }
      j++;
    }
    if(index == sizeof(LK_RESET_RE)){UART_puts(UART_GNSS, "yay");}
      else{UART_puts(UART_GNSS, "eh");}
    UART_putb(UART_GNSS, data, sizeof(data));
    for(int i = 0;i<sizeof(LK_RESET_RE);i++){
      sprintf(output, "0x%02"PRIx8" ", data[i]);
      UART_puts(UART_GNSS, output);
    }
    Delay_ms(3000);
    int index2 = 0;
    UART_putb(UART_CAMERA,LK_RESOLUTION_1600, sizeof(LK_RESOLUTION_1600));
    Delay_ms(25);
    while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&data2[z]);
      if(data2[z] == LK_RESOLUTION_RE[index2]){
        index2++;
      }else{
        index2 = 0;
      }
      z++;
    }
    if(index2 == sizeof(LK_RESOLUTION_RE)){UART_puts(UART_GNSS, "yay");}
    else{ sprintf(output, "%d", index2); UART_puts(UART_GNSS, output);}
    UART_putb(UART_GNSS, data2, sizeof(data2));
    for(int i = 0;i<sizeof(LK_RESOLUTION_RE);i++){
      sprintf(output, "0x%02"PRIx8" ", data2[i]);
      UART_puts(UART_GNSS, output);
    }
    LED_off(LED_B);
    /* On period */
    Delay_ms(1000);
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