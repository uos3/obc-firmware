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
#include "../../firmware.h"
#include <inttypes.h>

//-------------------------------------CAMERA COMMANDS-----------------------------------------------
static char LK_RESET[]     = {0x56, 0x00, 0x26, 0x00};
static char LK_RESET_RE[]  = {0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};

//baudrate settings -> datasheet
//static char LK_BAUDRATE_9600[] = {0x56, 0x00, 0x24, 0x03, 0x01, 0xAE};
//static char LK_BAUDRATE_19200[]  = {0x56, 0x00, 0x24, 0x03, 0x01, 0x56, 0xe4};
//static char LK_BAUDRATE_38400[]  = {0x56, 0x00, 0x24, 0x03, 0x01, 0x2a, 0xf2};
//static char LK_BAUDRATE_115200[] = {0x56, 0x00, 0x24, 0x03, 0x01, 0x0D, 0xA6};
//static char LK_BAUDRATE_RE[] = {0x76, 0x00, 0x24, 0x00, 0x00};

//resolution settings -> datasheet - value specific for each resolution in last byte!
static char LK_RESOLUTION[] = {0x56, 0x00, 0x54, 0x01, 0x00};
//static char LK_RESOLUTION_160[] = {0x56, 0x00, 0x54, 0x01, 0x22};
//static char LK_RESOLUTION_640[] = {0x56, 0x00, 0x54, 0x01, 0x00};
//static char LK_RESOLUTION_800[] = {0x56, 0x00, 0x54, 0x01, 0x1D};
//static char LK_RESOLUTION_1280[] = {0x56, 0x00, 0x54, 0x01, 0x1B};
//static char LK_RESOLUTION_1600[] = {0x56, 0x00, 0x54, 0x01, 0x21};
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
//function to write the command to the camera
#define CAMWRITE(a) UART_putb(UART_CAMERA,a,sizeof(a));
//function to check the actual response of the camera with the pattern
//for the time specified by the timeout it is looking for the response characters in UART line and matching them with the patter; return true if the response is matched
static bool UART_waitmatch(char *string, uint32_t string_length, uint32_t timeout)
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
//function for getting the jpegsize, which is four bytes after the LK_JPEGSIZE_RE - so it reads four bytes
static uint32_t UART_getw4(uint8_t serial)
{
  char c1=UART_getc(serial),c2=UART_getc(serial),c3=UART_getc(serial),c4=UART_getc(serial);
  return (uint32_t)((c1 << 24) | (c2 << 16) | (c3 << 8) | (c4 << 0)); //shifting values of read char/hex numbers to create one 32bit number
}
//function for sending the command to the camera and checking if it was correctly send by matching the response with UART_waitmatch function
static bool Camera_command(char *command, uint32_t command_length, char *response, uint32_t response_length)
{
  uint32_t attempts = 1;
  UART_putb(UART_CAMERA,command,command_length);
  while(!UART_waitmatch(response, response_length, 3000))
  {
    if(--attempts == 0)     //if no. of attempts = 1 and the response will not be matched -> function return false
    {                       //if no. of attempts > 1 -> the command will be send again
      return false;         //if UART_waitmatch will give true then we will not enter while loop and return true
    }
    Delay_ms(1000);
    UART_putb(UART_CAMERA,command,command_length);
  } 
  return true;
}
//function for setting the resolution of the image according to the chosen configuration
void set_resolution(image_acquisition_profile_t resolution){
  switch (resolution)
  {
/* Set the resolution of the picture according to the configuration file */
    case image_acquisition_profile_1600x1200:
          LK_RESOLUTION[4] = 0x21;
          break;
    case image_acquisition_profile_1280x960:
          LK_RESOLUTION[4] = 0x1B;
          break;
    case image_acquisition_profile_800x600:
          LK_RESOLUTION[4] = 0x1d;
          break;
    case image_acquisition_profile_640x480:
          LK_RESOLUTION[4] = 0x00;
          break;
    default:
          LK_RESOLUTION[4] = 0x00;
  }
}
//main driver function - uses above functions to set the camera, capture the image and store it in the FRAM memory
bool Camera_capture(uint32_t *picture_size, uint16_t* no_of_pages, image_acquisition_profile_t resolution)
{
  //pagesize must be multiple of 8 - camera requirement, size of FRAM slot is 87bytes - it is multiplied by 24
  //to obtain a number around 2000 - in this case 2088 exactly -> this was found during camera tests with demo_camera_dirty
  //for the small pagesize - like 106 - the camera send only 20 pages of information, the same happened for the pagesize around 1000
  //it seems to be a camera limit for that numbers - therefore I increased pagesize to around 2000 and for this number there was no problem
  //with reading the characters, I think I read even more than 20pages - I have no idea why for the smaller pages it was not working
  //if you will have the problem with reading 1600 resolution picture, try doubling pagesize - when you edit pagesize make sure
  //that the size of page_buffer array is the same like pagesize
  uint16_t pagesize = 0x828;   
  uint8_t begin_marker[5]={0}, end_marker[6]={0};
  uint8_t count_begin_marker = 0, count_end_marker = 0, count_picture_data = 0;
  uint8_t page_buffer[2088]={0};
  uint32_t jpegsize;                                       //stores the size of the image
  uint32_t jpeg_adress = 0x0000;                           //initial adress of the camera memory from which we reed the data
  uint64_t timestamp;
  bool endflag = false;
  // initialise camera
  if(!Camera_command(LK_RESET, sizeof(LK_RESET), LK_RESET_RE, sizeof(LK_RESET_RE)))
  {
    #ifdef DEBUG_PRINT
    UART_puts(UART_INTERFACE, "Reset Error \r\n");
    #endif
    return false;
  }
  Delay_ms(3000); // 2-3 sec gap required by data sheet before camera ready
  // set resolution
  set_resolution(resolution);
  if(!Camera_command(LK_RESOLUTION, sizeof(LK_RESOLUTION),LK_RESOLUTION_RE, sizeof(LK_RESOLUTION_RE)))
  {
    #ifdef DEBUG_PRINT
    UART_puts(UART_INTERFACE, "Resolution Error \r\n");
    #endif
    return false; 
  }
  Delay_ms(200);
  // set compression 
  if(!Camera_command(LK_COMPRESSION, sizeof(LK_COMPRESSION), LK_COMPRESSION_RE, sizeof(LK_COMPRESSION_RE)))
  {
    #ifdef DEBUG_PRINT
    UART_puts(UART_INTERFACE, "Compression Error \r\n");
    #endif
    return false;
  }
  Delay_ms(200);
  // take picture
  if(!Camera_command(LK_PICTURE, sizeof(LK_PICTURE), LK_PICTURE_RE, sizeof(LK_PICTURE_RE)))
  {
    #ifdef DEBUG_PRINT
    UART_puts(UART_INTERFACE, "Take picture error \r\n");
    #endif
    return false;
  }
  Delay_ms(200);
  // read size
  if(!Camera_command(LK_JPEGSIZE, sizeof(LK_JPEGSIZE),LK_JPEGSIZE_RE, sizeof(LK_JPEGSIZE_RE)))
  { 
    #ifdef DEBUG_PRINT
    UART_puts(UART_INTERFACE, "Pictrue size error \r\n");
    #endif
    return false;
  }
  jpegsize = UART_getw4(UART_CAMERA);
  Delay_ms(200);
 //unnecessary for the operation, just for the debugging, takes time to print----------------------------
  #ifdef DEBUG_PRINT
  char jp_output[100];
  sprintf(jp_output, "jpeg size is: %d\r\n", jpegsize); //output of the sprintf is the char buffer specified in first argument
  UART_puts(UART_INTERFACE, jp_output); //print this message about size of jpeg
  #endif
 //-----------------------------------------------------------------------------------------------------
  // write length to obtain +8 bytes?
  LK_READPICTURE[12] = 0x08;
  LK_READPICTURE[13] = 0x20;    //number of bits we will be reading in each camera data packet
  // 0.1ms
  LK_READPICTURE[14] = LK_PICTURE_TIME_dot1ms[0];
  LK_READPICTURE[15] = LK_PICTURE_TIME_dot1ms[1];
  //camera response structure: 0x76 0x00 0x32 0x00 x00 <data> 0x76 0x76 0x00 0x32 0x00 0x00
  while(!endflag)
  {
    LK_READPICTURE[8] = jpeg_adress/0x100;  //should give the possibility to make 630 readings
    LK_READPICTURE[9] = jpeg_adress%0x100;  //but may need be extended to bytes 6 and 7 if the smaller pagesize is required
    UART_putb(UART_CAMERA,LK_READPICTURE,sizeof(LK_READPICTURE));
    RTC_getTime_ms(&timestamp);
    while(!RTC_timerElapsed_ms(timestamp,2000)){
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
              count_picture_data++;
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

  /* Need to divide the 2080 characters into the portions of 104 characters to match the size of FRAM_buffer slot */
  uint8_t j = 0;                                //variable to support operation
  uint8_t storage_buffer[BUFFER_SLOT_SIZE/8];   //buffer which will be sent directly to the FRAM
  for(int i = 0; i<pagesize; i++){              //run the for loop through all the elements of the data read from the camera
    storage_buffer[j] = page_buffer[i];         //copy the data to the storage_buffer
    j++;                                        
    if(j == (BUFFER_SLOT_SIZE/8)){              //if j match the size of the BUFFER slot size, send the storage_buffer to the FRAM and reset j to 0 for the next portion
      j = 0;
      Buffer_store_new_data(storage_buffer);
    }                                           //repeat that until all the page data is portioned and stored in FRAM
  }

  count_begin_marker = 0;
  count_picture_data = 0;
  count_end_marker = 0;
  jpeg_adress += pagesize;
  }
  *no_of_pages = ROUND_UP(jpegsize/pagesize ,1);  //TODO: replace ROUND_UP (not working) with expression to always round up the no_of_pages
  *picture_size = jpegsize;
  return true;
}