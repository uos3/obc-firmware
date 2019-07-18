/**
 * @ingroup uos3-proto
 * @ingroup camera
 *
 * @file uos3-proto/camera.c
 * @brief Camera Hardware Driver - uos3-proto board
 *
 * @{
 */

#include "board.h"
#include "../uart.h"
#include "../rtc.h"
#include "../delay.h"
#include "../camera.h"
//#include "../../buffer/buffer.h"
#include <inttypes.h>


static char LK_RESET[]     = {0x56, 0x00, 0x26, 0x00};
static char LK_RESET_RE[]    = {0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};  //legit!

//commands for the camera baudrate settings -> datasheet
//static char LK_BAUDRATE_9600[] = {0x56, 0x00, 0x24, 0x03, 0x01, 0xAE};
//static char LK_BAUDRATE_19200[]  = {0x56, 0x00, 0x24, 0x03, 0x01, 0x56, 0xe4};
//static char LK_BAUDRATE_38400[]  = {0x56, 0x00, 0x24, 0x03, 0x01, 0x2a, 0xf2};
//static char LK_BAUDRATE_115200[] = {0x56, 0x00, 0x24, 0x03, 0x01, 0x0D, 0xA6};
//static char LK_BAUDRATE_RE[] = {0x76, 0x00, 0x24, 0x00, 0x00};

//commands for the camera resolution settings -> datasheet
//static char LK_RESOLUTION_VGA[] = {0x56, 0x00, 0x54, 0x01, 0x00};
static char LK_RESOLUTION_160[] = {0x56, 0x00, 0x54, 0x01, 0x22};
static char LK_RESOLUTION_800[] = {0x56, 0x00, 0x54, 0x01, 0x1D};
static char LK_RESOLUTION_1280[] = {0x56, 0x00, 0x54, 0x01, 0x1B};
static char LK_RESOLUTION_1600[] = {0x56, 0x00, 0x54, 0x01, 0x21};
static char LK_RESOLUTION_RE[] = {0x76, 0x00, 0x54, 0x00, 0x00}; // v T; response from the camera after setting the resolution

//setting compression rate -> value of compression in last HEX number
static char LK_COMPRESSION[] = {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04, 0x10}; // Value in last byte
static char LK_COMPRESSION_RE[] = {0x76, 0x00, 0x31, 0x00, 0x00}; // value in last byte

static char LK_PICTURE[]   = {0x56, 0x00, 0x36, 0x01, 0x00};
static char LK_PICTURE_RE[]    = {0x76, 0x00, 0x36, 0x00, 0x00};
static char LK_JPEGSIZE[]    = {0x56, 0x00, 0x34, 0x01, 0x00};
static char LK_JPEGSIZE_RE[] = {0x76, 0x00, 0x34, 0x00, 0x04}; //, 0x00, 0x00, 0x00, 0x00}; // then XH XL // assumption is last 4 bytes are size
//static char LK_STOP[]    = {0x56, 0x00, 0x36, 0x01, 0x03};
//static char LK_STOP_RE[]   = {0x76, 0x00, 0x36, 0x00, 0x00};

static char LK_READPICTURE[] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00};
static char LK_PICTURE_TIME_dot1ms[]  = {0x00, 0x0a}; // 0.1 ms
static char LK_READPICTURE_RE[]  = {0x76, 0x00, 0x32, 0x00, 0x00};
static char JPEG_START[]     = {0xFF, 0xD8};
static char JPEG_END[]   = {0xFF, 0xD9};

#define CAMWRITE(a) UART_putb(UART_CAMERA,a,sizeof(a)); // send this message

static bool UART_waitmatch(char *string, uint32_t string_length, uint32_t timeout)  ///use this legit!
{
  char c;
  uint32_t index=0;
  uint64_t start_timestamp;
  RTC_getTime_ms(&start_timestamp);
  while(!RTC_timerElapsed_ms(start_timestamp, timeout))
  {
    if(UART_charsAvail(UART_CAMERA)){
      if(UART_getc_nonblocking(UART_CAMERA, &c))
      {
        if(c == string[index])
        {
          index++;
          if(index == string_length)
          {
            return true;
          }
        }
        else
        {
          index = 0;
        }
      }
    }
  }
  return false;
}
/*
static bool UART_command_match(char *string, uint32_t string_length)
{
  char data_in;
  uint8_t index = 0;
  while(UART_charsAvail(UART_CAMERA))
  {
    if(UART_getc_nonblocking(UART_CAMERA, &data_in)){
      if(data_in == string[index])
      {
        index++;
      }
      else
      {
        index = 0;
      }
    }
  }
  if(index == string_length){ return true;}
  else{ return false;}
}*/

static uint32_t UART_getw4(uint8_t serial)
{
  char c1=UART_getc(serial),c2=UART_getc(serial),c3=UART_getc(serial),c4=UART_getc(serial);
  return (uint32_t)((c1 << 24) | (c2 << 16) | (c3 << 8) | (c4 << 0)); //shifting values of read char/hex numbers to create one 32bit number
}

static bool Camera_command(char *command, uint32_t command_length, char *response, uint32_t response_length)
{
  uint32_t attempts = 1;
  UART_putb(UART_CAMERA,command,command_length);
  while(!UART_waitmatch(response, response_length, 3000))
  {
    if(--attempts == 0)     //why --attempts; as attempts = 1; so ut will make if true, and funtion will return false
    {                       //how it works - UART_waitmatch is looking for returning string matching the return command;
      return false;         //if it is correct then this function return true adn command is successfully send; if return false (not matched for specified waiting time), this function is returning false, as we have only one try
    }
    Delay_ms(1000);
    UART_putb(UART_CAMERA,command,command_length);
  } 
  return true;
}
/*
static bool Camera_command(char *command, char *response)
{
  uint32_t attempts = 1;
  CAMWRITE(command);
  Delay_ms(25);
  if(!UART_command_match(response, sizeof(response)))
  {
    if(--attempts == 0)     //why --attempts; as attempts = 1; so ut will make if true, and funtion will return false
    {                       //how it works - UART_waitmatch is looking for returning string matching the return command;
      return false;         //if it is correct then this function return true adn command is successfully send; if return false (not matched for specified waiting time), this function is returning false, as we have only one try
    }
    Delay_ms(1000);
    CAMWRITE(command);
  }else
  { 
  return true;
  }
}*/

bool Camera_capture(uint32_t *picture_size, uint16_t* no_of_pages, uint8_t deb_uart_num,bool if_print)
{
  uint8_t pagesize = 104;//((uint8_t)(BUFFER_SLOT_SIZE/64))*8;   //because it must be multiple of 8, size of the page - correspond to size of the FRAM slot
  uint8_t begin_marker[5], end_marker[6];
  uint8_t count_begin_marker = 0, count_end_marker = 0, count_picture_data = 0;
  uint8_t page_buffer[pagesize];
  uint32_t jpegsize;                                       //stores the size of the image
  uint32_t jpeg_adress = 0x0000;                           //initial adress of the camera memory from which we reed the data
  uint64_t timestamp;
  bool endflag = false;
  // initialise camera
  if(!Camera_command(LK_RESET, sizeof(LK_RESET), LK_RESET_RE, sizeof(LK_RESET_RE)))
  {
    UART_puts(deb_uart_num, "Reset Error \r\n");
    return false;
  }
  Delay_ms(3000); // 2-3 sec gap required by data sheet before camera ready
  // set resolution
  if(!Camera_command(LK_RESOLUTION_1600, sizeof(LK_RESOLUTION_1600),LK_RESOLUTION_RE, sizeof(LK_RESOLUTION_RE)))
  {
    UART_puts(deb_uart_num, "Resolution Error \r\n");
    return false; 
  }
  // set compression 
  if(!Camera_command(LK_COMPRESSION, sizeof(LK_COMPRESSION), LK_COMPRESSION_RE, sizeof(LK_COMPRESSION_RE)))
  {
    UART_puts(deb_uart_num, "Compression Error \r\n");
    return false;
  }
  // take picture
  if(!Camera_command(LK_PICTURE, sizeof(LK_PICTURE), LK_PICTURE_RE, sizeof(LK_PICTURE_RE)))
  {
    UART_puts(deb_uart_num, "Take picture error \r\n");
    return false;
  }
  // read size
  if(!Camera_command(LK_JPEGSIZE, sizeof(LK_JPEGSIZE),LK_JPEGSIZE_RE, sizeof(LK_JPEGSIZE_RE)))
  {
    UART_puts(deb_uart_num, "Pictrue size error \r\n");
    return false;
  }
  jpegsize = UART_getw4(UART_CAMERA);
 //unnecessary for the operation, just for the debugging, takes time to print----------------------------
  char jp_output[100];
  sprintf(jp_output, "jpeg size is: %d\r\n", jpegsize); //output of the sprintf is the char buffer specified in first argument
  UART_puts(deb_uart_num, jp_output); //print this message about size of jpeg
 //-----------------------------------------------------------------------------------------------------
  // offset in file start (0 here) - place where in the command is starting address
  //LK_READPICTURE[6] = 0x00;
  //LK_READPICTURE[7] = 0x00;
  //LK_READPICTURE[8] = 0x00;
  //LK_READPICTURE[9] = 0x00;
  // write length to obtain +8 bytes?
  LK_READPICTURE[13] = pagesize;    //number of bites we will be reading in each camera data packet
  // 0.1ms
  LK_READPICTURE[14] = LK_PICTURE_TIME_dot1ms[0];
  LK_READPICTURE[15] = LK_PICTURE_TIME_dot1ms[1];
  //camera response structure: 0x76 0x00 0x32 0x00 x00 <data> 0x76 0x76 0x00 0x32 0x00 0x00
  while(!endflag)
  {
    LK_READPICTURE[8] = jpeg_adress / 256;  //should give the possibility to make 630 readings
    LK_READPICTURE[9] = jpeg_adress % 256;  //but may need be extended to bytes 6 and 7
    CAMWRITE(LK_READPICTURE);
    RTC_getTime_ms(&timestamp);
    while(!RTC_timerElapsed_ms(timestamp,100)){
      if(UART_charsAvail(UART_CAMERA)){
        if(count_begin_marker<5){                                  //read first five characters which are response markers
            UART_getc_nonblocking(UART_CAMERA, &begin_marker[count_begin_marker]);
            count_begin_marker++; 
        }
        else if(count_begin_marker>=5 && count_picture_data<pagesize && !endflag)
        {
          UART_getc_nonblocking(UART_CAMERA, &page_buffer[count_picture_data]);
          if((page_buffer[count_picture_data-1]==0xFF) && (page_buffer[count_picture_data]==0xd9)){
              endflag = true;
          }
          else
          {
          count_picture_data++;
          }
            
        }
        else
        {
          UART_getc_nonblocking(UART_CAMERA, &end_marker[count_end_marker]);
          count_end_marker++;
        }
      }
    }
  //Buffer_store_new_data(page_buffer);
  if(if_print){ print_to_terminal(deb_uart_num, page_buffer, count_picture_data); }
  count_begin_marker = 0;
  count_picture_data = 0;
  count_end_marker = 0;
  jpeg_adress += pagesize;
  }
  *picture_size = jpegsize;
  *no_of_pages = jpegsize/pagesize +1;
  return true;
}
void print_to_terminal(uint8_t uart_num,uint8_t* data, uint8_t count){
  char* output = malloc(20*sizeof(char));
  for(int i =0;i<=count;i++){
    sprintf(output, "0x%02"PRIx8" ",data[i]);
      UART_puts(uart_num, output);
  }
  UART_puts(uart_num, "\r\n");
  free(output);
}