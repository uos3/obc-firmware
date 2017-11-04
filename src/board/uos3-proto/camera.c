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
#include "../delay.h"

#include "../camera.h"

//LK_POWERUP[] = {0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};
char LK_POWERUP[] = {0x0d, 0x0a, 0x00, 0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a}; // Init end
//LK_POWERUP[] = {0x0d,0x0a,0,4,0,0x49,0x68,0x69,0x68,0x20,0x65,0xc,0};

char LK_BAUDRATE_19200[]  ={0x56, 0x00, 0x24, 0x03, 0x01, 0x56, 0xe4};
char LK_BAUDRATE_38400[]  = {0x56, 0x00, 0x24, 0x03, 0x01, 0x2a, 0xf2};
char LK_BAUDRATE_RE[] = {0x76, 0x00, 0x24, 0x00, 0x00};
 
char LK_RESOLUTION_VGA[] = {0x56, 0x00, 0x54, 0x01, 0x00};
char LK_RESOLUTION_800[] = {0x56, 0x00, 0x54, 0x01, 0x1D};
char LK_RESOLUTION_1280[] = {0x56, 0x00, 0x54, 0x01, 0x1B};
char LK_RESOLUTION_1600[] = {0x56, 0x00, 0x54, 0x01, 0x21};
char LK_RESOLUTION_RE[] = {0x76, 0x00, 0x54, 0x00, 0x00}; // v T

char LK_COMPRESSION[] = {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04};
char LK_COMPRESSION_RE[] = {0x76, 0x00, 0x31, 0x00, 0x00}; // v 1

char LK_RESET[]     = {0x56, 0x00, 0x26, 0x00};
// OLD: LK_RESET_RE[]     = a2s([0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a])
// LEGIT: LK_RESET_RE[]     = a2s([0x76,0x00,0x31,0x00,0x00])
char LK_RESET_RE[]    = {0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};

char LK_PICTURE[]   = {0x56, 0x00, 0x36, 0x01, 0x00};
char LK_PICTURE_RE[]    = {0x76, 0x00, 0x36, 0x00, 0x00};
char LK_JPEGSIZE[]    = {0x56, 0x00, 0x34, 0x01, 0x00};
char LK_JPEGSIZE_RE[] = {0x76, 0x00, 0x34, 0x00, 0x04}; //, 0x00, 0x00, 0x00, 0x00}; // then XH XL // assumption is last 4 bytes are size
char LK_STOP[]    = {0x56, 0x00, 0x36, 0x01, 0x03};
char LK_STOP_RE[]   = {0x76, 0x00, 0x36, 0x00, 0x00};

char LK_READPICTURE[] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A};
char LK_PICTURE_TIME[]  = {0x00, 0x0A}; // .1 ms
char LK_READPICTURE_RE[]  = {0x76, 0x00, 0x32, 0x00, 0x00};
char JPEG_START[]     = {0xFF, 0xD8};
char JPEG_END[]   = {0xFF, 0xD9};

#define CAMWRITE(a) UART_putb(UART_CAM_HEADER,a,sizeof(a)); // send this message

#define CAMREAD(a) while (!match_string(a,sizeof(a))) {} // wait for this sequence (forever if necessary)

static uint32_t UART_getw4(uint8_t serial)
{
  char c1=UART_getc(serial),c2=UART_getc(serial),c3=UART_getc(serial),c4=UART_getc(serial);
  return (uint32_t)((c1 << 24) | (c2 << 16) | (c3 << 8) | (c4 << 0));
}

static void UART_putw4(uint8_t serial, uint32_t w)
{
  UART_putc(serial, (char)((w >> 24) & 0xFF));
  UART_putc(serial, (char)((w >> 16) & 0xFF));
  UART_putc(serial, (char)((w >> 8) & 0xFF));
  UART_putc(serial, (char)(w & 0xFF));
}

static bool match_string(char *data, uint32_t len)
{
  char c;
  uint32_t i;
  for(i=0;i<len;i++)
  {
    c=UART_getc(UART_CAM_HEADER);
    if (c!=data[i]) return false;
  }
  return true;
}

bool Camera_capture(uint32_t page_size, void (*page_store)(uint8_t*,uint32_t))
{
  uint32_t jpegsize, i=0, j=0, endfoundcount = 0;
  uint8_t page_buffer[page_size];

  // initialise camera
  CAMWRITE(LK_RESET);
  CAMREAD(LK_RESET_RE); 

  Delay_ms(1000); // 2-3 sec gap required by data sheet before camera ready

  // set resolution
  CAMWRITE(LK_RESOLUTION_1600);
  CAMREAD(LK_RESOLUTION_RE);

  // set compression
  CAMWRITE(LK_COMPRESSION);
  UART_putc(UART_CAM_HEADER,0x10);
  CAMREAD(LK_COMPRESSION_RE);

  // take picture
  CAMWRITE(LK_PICTURE);
  CAMREAD(LK_PICTURE_RE);

  // read size
  CAMWRITE(LK_JPEGSIZE);
  CAMREAD(LK_JPEGSIZE_RE);
  jpegsize = UART_getw4(UART_CAM_HEADER); // file size (lowest 32 bits)

  CAMWRITE(LK_READPICTURE);
  UART_putw4(UART_CAM_HEADER, 0); // offset in file start (0 here)
  UART_putw4(UART_CAM_HEADER, jpegsize-1); // write length to obtain +8 bytes?
  CAMWRITE(LK_PICTURE_TIME);
  CAMREAD(LK_READPICTURE_RE);

  for(i=0; i<jpegsize && endfoundcount<2;i++)
  {
    page_buffer[j] = UART_getc(UART_CAM_HEADER);

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
