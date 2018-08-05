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

//LK_POWERUP[] = {0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};
//static char LK_POWERUP[] = {0x0d, 0x0a, 0x00, 0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a}; // Init end
//LK_POWERUP[] = {0x0d,0x0a,0,4,0,0x49,0x68,0x69,0x68,0x20,0x65,0xc,0};

//static char LK_BAUDRATE_19200[]  = {0x56, 0x00, 0x24, 0x03, 0x01, 0x56, 0xe4};
//static char LK_BAUDRATE_38400[]  = {0x56, 0x00, 0x24, 0x03, 0x01, 0x2a, 0xf2};
//static char LK_BAUDRATE_RE[] = {0x76, 0x00, 0x24, 0x00, 0x00};
 
//static char LK_RESOLUTION_VGA[] = {0x56, 0x00, 0x54, 0x01, 0x00};
//static char LK_RESOLUTION_800[] = {0x56, 0x00, 0x54, 0x01, 0x1D};
//static char LK_RESOLUTION_1280[] = {0x56, 0x00, 0x54, 0x01, 0x1B};
static char LK_RESOLUTION_1600[] = {0x56, 0x00, 0x54, 0x01, 0x21};
static char LK_RESOLUTION_RE[] = {0x76, 0x00, 0x54, 0x00, 0x00}; // v T

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
static char LK_PICTURE_TIME_dot1ms[]  = {0x00, 0x0A}; // .1 ms
static char LK_READPICTURE_RE[]  = {0x76, 0x00, 0x32, 0x00, 0x00};
//static char JPEG_START[]     = {0xFF, 0xD8};
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
  return (uint32_t)((c1 << 24) | (c2 << 16) | (c3 << 8) | (c4 << 0));
}

static bool Camera_command(char *command, uint32_t command_length, char *response, uint32_t response_length)
{
  uint32_t attempts = 1;

  UART_putb(UART_CAMERA,command,command_length);
  while(!UART_waitmatch(response, response_length, 3000))
  {
    if(--attempts == 0)
    {
      return false;
    }
    Delay_ms(1000);
    UART_putb(UART_CAMERA,command,command_length);
  } 
  return true;
}

bool Camera_capture(uint32_t page_size, void (*page_store)(uint8_t*,uint32_t))
{
  uint32_t jpegsize, i=0, j=0, endfoundcount = 0;
  uint8_t page_buffer[page_size];

  // initialise camera
  if(!Camera_command(LK_RESET, sizeof(LK_RESET), LK_RESET_RE, sizeof(LK_RESET_RE)))
  {
    return false;
  }

  Delay_ms(2000); // 2-3 sec gap required by data sheet before camera ready

  // set resolution
  if(!Camera_command(LK_RESOLUTION_1600, sizeof(LK_RESOLUTION_1600), LK_RESOLUTION_RE, sizeof(LK_RESOLUTION_RE)))
  {
    return false;
  }

  // set compression
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
  jpegsize = UART_getw4(UART_CAMERA); // file size (lowest 32 bits)

  // offset in file start (0 here)
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

  for(i=0; i<jpegsize && endfoundcount<2;i++)
  {
    page_buffer[j] = UART_getc(UART_CAMERA);

    if(page_buffer[j] == JPEG_END[endfoundcount])
    {
      endfoundcount+=1;
    }
    else
    {
      endfoundcount=0;
    }

    j++;
    if(j==page_size || endfoundcount==2)
    {
      page_store(page_buffer, j);
      j=0;
    }
  }

  return true;
}
