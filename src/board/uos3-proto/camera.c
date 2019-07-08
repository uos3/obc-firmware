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
//#include "../fram.h"
//#include "/buffer/buffer.h"

#include "../camera.h"

//LK_POWERUP[] = {0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};
//static char LK_POWERUP[] = {0x0d, 0x0a, 0x00, 0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a}; // Init end
//LK_POWERUP[] = {0x0d,0x0a,0,4,0,0x49,0x68,0x69,0x68,0x20,0x65,0xc,0};

//commands for the camera baudrate settings -> datasheet
//static char LK_BAUDRATE_19200[]  = {0x56, 0x00, 0x24, 0x03, 0x01, 0x56, 0xe4};
//static char LK_BAUDRATE_38400[]  = {0x56, 0x00, 0x24, 0x03, 0x01, 0x2a, 0xf2};
//static char LK_BAUDRATE_RE[] = {0x76, 0x00, 0x24, 0x00, 0x00};

//commands for the camera resolution settings -> datasheet
//static char LK_RESOLUTION_VGA[] = {0x56, 0x00, 0x54, 0x01, 0x00};
static char LK_RESOLUTION_160[] = {0x56, 0x00, 0x54, 0x01, 0x22};
//static char LK_RESOLUTION_800[] = {0x56, 0x00, 0x54, 0x01, 0x1D};
//static char LK_RESOLUTION_1280[] = {0x56, 0x00, 0x54, 0x01, 0x1B};
//static char LK_RESOLUTION_1600[] = {0x56, 0x00, 0x54, 0x01, 0x21};
static char LK_RESOLUTION_RE[] = {0x76, 0x00, 0x54, 0x00, 0x00}; // v T; response from the camera after setting the resolution

//setting compression rate -> value of compression in last HEX number
static char LK_COMPRESSION[] = {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04, 0x00}; // Value in last byte
static char LK_COMPRESSION_RE[] = {0x76, 0x00, 0x31, 0x00, 0x00}; // v 1

static char LK_RESET[]     = {0x56, 0x00, 0x26, 0x00};
// OLD: LK_RESET_RE[]     = a2s([0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a])
// LEGIT: LK_RESET_RE[]     = a2s([0x76,0x00,0x31,0x00,0x00])
static char LK_RESET_RE[]    = {0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};

static char LK_PICTURE[]   = {0x56, 0x00, 0x36, 0x01, 0x00};
static char LK_PICTURE_RE[]    = {0x76, 0x00, 0x36, 0x00, 0x00};
static char LK_JPEGSIZE[]    = {0x56, 0x00, 0x34, 0x01, 0x00};
static char LK_JPEGSIZE_RE[] = {0x76, 0x00, 0x34, 0x00, 0x04}; //, 0x00, 0x00, 0x00, 0x00}; // then XH XL // assumption is last 4 bytes are size
//static char LK_STOP[]    = {0x56, 0x00, 0x36, 0x01, 0x03};
//static char LK_STOP_RE[]   = {0x76, 0x00, 0x36, 0x00, 0x00};

static char LK_READPICTURE[] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00};
static char LK_PICTURE_TIME_dot1ms[]  = {0x00, 0x0A}; // 0.1 ms
static char LK_READPICTURE_RE[]  = {0x76, 0x00, 0x32, 0x00, 0x00};
static char JPEG_START[]     = {0xFF, 0xD8};
static char JPEG_END[]   = {0xFF, 0xD9};

#define CAMWRITE(a) UART_putb(UART_CAMERA,a,sizeof(a)); // send this message

static bool UART_waitmatch(char *string, uint32_t string_length, uint32_t timeout)
{
  char c;
  uint32_t index;
  uint64_t start_timestamp;
  RTC_getTime_ms(&start_timestamp);

  index = 0;
  while(!RTC_timerElapsed_ms(start_timestamp, timeout))
  {
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
  return false;
}

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

bool Camera_capture(uint32_t page_size, uint32_t *image_length, uint32_t *number_of_pages, uint8_t *total_data)
{
  uint32_t jpegsize=0;                  //storing the total size of the jpeg
  uint32_t i = 0;                     //variable for the for loop which is controlling the total jpeg data
  uint32_t j = 0;                     //variable for the for loop which is controlling the page jpeg data
  uint32_t endfoundcount = 0;         //variable used for finding the end of the jpeg : FF D9
  uint32_t page_count = 1;            //variable for counting number of pages, initialised as 1, as we start on the first page
  uint32_t startfoundcount = 0;
  // initialise camera
  if(!Camera_command(LK_RESET, sizeof(LK_RESET), LK_RESET_RE, sizeof(LK_RESET_RE)))
  {
    return false;
  }

  Delay_ms(2000); // 2-3 sec gap required by data sheet before camera ready

  // set resolution
  if(!Camera_command(LK_RESOLUTION_160, sizeof(LK_RESOLUTION_160), LK_RESOLUTION_RE, sizeof(LK_RESOLUTION_RE)))
  {
    return false;
  }

  // set compression
  //LK_COMPRESSION[8] = 0x66; //for demo only, without the FRAM
  LK_COMPRESSION[8] = 0x10;
  if(!Camera_command(LK_COMPRESSION, sizeof(LK_COMPRESSION), LK_COMPRESSION_RE, sizeof(LK_COMPRESSION_RE)))
  {
    return false;
  }

  // take picture
  if(!Camera_command(LK_PICTURE, sizeof(LK_PICTURE), LK_PICTURE_RE, sizeof(LK_PICTURE_RE)))
  {
    return false;
  }

  // read size
  if(!Camera_command(LK_JPEGSIZE, sizeof(LK_JPEGSIZE), LK_JPEGSIZE_RE, sizeof(LK_JPEGSIZE_RE)))
  {
    return false;
  }
  jpegsize = UART_getw4(UART_CAMERA); // file size (lowest 32 bits), sizeof char is 8bits which is 0x?? HEX number
 // so after matching the read size response, next four characters will be size of picture

 //unnecessary for the operation, just for the debugging, takes time to print----------------------------
  char jp_output[100];
  sprintf(jp_output, "jpeg size is: %d\r\n", jpegsize); //output of the sprintf is the char buffer specified in first argument
  UART_puts(UART_GNSS, jp_output); //print this message about size of jpeg
 //------------------------------------------------------------------------------------------------------

  // offset in file start (0 here) - place where in the command is starting address
  LK_READPICTURE[6] = 0x00;
  LK_READPICTURE[7] = 0x00;

  LK_READPICTURE[8] = 0x00;
  LK_READPICTURE[9] = 0x00;

  // write length to obtain +8 bytes?
  LK_READPICTURE[10] = (char)((jpegsize >> 24) & 0xFF);
  LK_READPICTURE[11] = (char)((jpegsize >> 16) & 0xFF);
  LK_READPICTURE[12] = (char)((jpegsize >> 8) & 0xFF);
  LK_READPICTURE[13] = (char)(jpegsize & 0xFF);

  // 0.1ms
  LK_READPICTURE[14] = LK_PICTURE_TIME_dot1ms[0];
  LK_READPICTURE[15] = LK_PICTURE_TIME_dot1ms[1];

  if(!Camera_command(LK_READPICTURE, sizeof(LK_READPICTURE), LK_READPICTURE_RE, sizeof(LK_READPICTURE_RE)))
  {
    return false;
  }
  //UART_puts(UART_GNSS, "In Camera driver\r\n");
  for(i=0; i<jpegsize && endfoundcount<2;i++)
  //while(endfoundcount<2)
  {
    //DELAYS THE READING OF CHARACTER  FROM CAMERA SO GETS STUCK IN LOOP
    //UART_puts(UART_EPS, "In Camera driver loop\r\n");
    total_data[j] = UART_getc(UART_CAMERA);  //characters in line
    if(total_data[j] == JPEG_END[endfoundcount])
    {
      endfoundcount+=1;
    }
    else
    {
      endfoundcount=0;
    }

    j++;
    if(j== (page_size*page_count) || endfoundcount==2)//if the j match the end of the pages or the end of the jpeg is found
    { 
      //Buffer_store_new_data(page_buffer); //need to be testes with the engineering board only, as the dev. one
      //for debugging only---------------
      //char i_output[100];
      //printf(i_output, "i output is: %d, j output is: %d, endfoundcount is: %d\r\n", i, j, endfoundcount);
      //UART_puts(UART_GNSS, i_output);
      //--------------------------------
      page_count++;               //increment, as the page is finished
    }
  }
  /*while (endfoundcount<2)
  {
      while (startfoundcount <2)  //wait for the starting character of the jpeg
      {
        total_data[j] = UART_getc(UART_CAMERA);           //get the character
        if(total_data[j] == JPEG_START[startfoundcount])  //check it the first/second starting character is found
        {
          startfoundcount+=1;                             //starting character found
          j++;                                            //increment j to get new character
        }
        else                                              //starting character not found, wait and keep looking
        {
          startfoundcount=0;
        }
      }

    total_data[j] = UART_getc(UART_CAMERA);             //get new characters; j = 2 after previous loop
      
      if(total_data[j] == JPEG_END[endfoundcount])      //check each character if it is ending character
      {
          endfoundcount+=1;
      }
      else
      {
          endfoundcount=0;
      }

    j++;
    if(j== (page_size*page_count) || endfoundcount==2)
    {
      char i_output[100];
      sprintf(i_output, "i output is: %d, j output is: %d, endfoundcount is: %d\r\n", i, j, endfoundcount);
      UART_puts(UART_GNSS, i_output);
      page_count++;  
    }
  }*/
  
  *image_length = j;        //length of the image to return
  *number_of_pages = page_count;   //number of the pages to return
  return true;
}

void camera_data_print(uint32_t number_of_pages, uint32_t page_size)
{
    int count1 = 0, count2 = 0;
    uint8_t printing_buffer[page_size];
    char *output = "Hello";
    while (count1 < number_of_pages){
  
        if(Buffer_get_next_data(printing_buffer))
        {   
          UART_puts(UART_GNSS, output);
        }
        count1++;
    }
}