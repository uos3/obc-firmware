
// camera test
//
// Suzanna Lucarotti (c) 01/09/2017
//
// for use with the UoS3 Cubesat
//


#include "../firmware.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include <stdlib.h>
#include <stdarg.h>

#define TRUE 1
#define FALSE 0

#define DEBUG_SERIAL UART_CAM_HEADER // this is the serial port debug messages will come through (UART_CAM_HEADER or UART_PC104_HEADER)
#define GPS_SERIAL UART_PC104_HEADER
#define CAM_SERIAL UART_CAM_HEADER

#define DEBUG_SERIAL GPS_SERIAL


//LK_POWERUP[] = {0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};
LK_POWERUP[] = {0x0d, 0x0a, 0x00, 0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};

//LK_POWERUP[] = {0x0d,0x0a,0,4,0,0x49,0x68,0x69,0x68,0x20,0x65,0xc,0};

LK_BAUDRATE_19200[]	={0x56, 0x00, 0x24, 0x03, 0x01, 0x56, 0xe4};
LK_BAUDRATE_38400[]	= {0x56, 0x00, 0x24, 0x03, 0x01, 0x2a, 0xf2};
LK_BAUDRATE_RE[]	= {0x76, 0x00, 0x24, 0x00, 0x00};
 
LK_RESOLUTION_VGA[] = {0x56, 0x00, 0x54, 0x01, 0x00};
LK_RESOLUTION_800[] = {0x56, 0x00, 0x54, 0x01, 0x1D};
LK_RESOLUTION_1280[] = {0x56, 0x00, 0x54, 0x01, 0x1B};
LK_RESOLUTION_1600[] = {0x56, 0x00, 0x54, 0x01, 0x21};
LK_RESOLUTION_RE[] = {0x76, 0x00, 0x54, 0x00, 0x00};

LK_COMPRESSION[] = {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04};
LK_COMPRESSION_RE[] = {0x76, 0x00, 0x31, 0x00, 0x00};

LK_RESET[] 		= {0x56, 0x00, 0x26, 0x00};
// OLD: LK_RESET_RE[] 		= a2s([0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a])
// LEGIT: LK_RESET_RE[] 		= a2s([0x76,0x00,0x31,0x00,0x00])
LK_RESET_RE[] 		= {0x76,0x00,0x54,0x00,0x00};
LK_RESET_RE2[] 		= {0x76,0x00,0x31,0x00,0x00};

LK_PICTURE[]		= {0x56, 0x00, 0x36, 0x01, 0x00};
LK_PICTURE_RE[]		= {0x76, 0x00, 0x36, 0x00, 0x00};
LK_JPEGSIZE[] 		= {0x56, 0x00, 0x34, 0x01, 0x00};
LK_JPEGSIZE_RE[]	= {0x76, 0x00, 0x34, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00}; // then XH XL
LK_STOP[]		= {0x56, 0x00, 0x36, 0x01, 0x03};
LK_STOP_RE[]		= {0x76, 0x00, 0x36, 0x00, 0x00};

LK_READPICTURE[]	= {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00};
LK_PICTURE_TIME[]	= {0x00, 0x0A}; // .1 ms
LK_READPICTURE_RE[] 	= {0x76, 0x00, 0x32, 0x00, 0x00};
JPEG_START[] 		= {0xFF, 0xD8};
JPEG_END[]		= {0xFF, 0xD9};


 
// Wrappers for printing to serial port - always to the one specified as output, placed here close to code for clarity

#define DISP3(x,y,z) UART_putstr(DEBUG_SERIAL,x,y,z); // x and z are strings, y is a number shown as signed base10 16bit
#define DISP2(y,z) UART_putstr(DEBUG_SERIAL,NULL,y,z); 
#define DISP1(x) UART_puts(DEBUG_SERIAL,x);

 //////////////////////////////////////////////////////////////////
 // the actual main code
 //////////////////////////////////////////////////////////////////

bool match_string(char *data)
 {
 if (UART_charsAvail<sizeof(data)) return FALSE; // skip if not enough in buffer
// int i=0;
  for (int i=0;i<sizeof(data);i++)
   {
    char c=UART_getc(CAM_SERIAL);
   if (i>0) {DISP3("[",c,"]");UART_putc(DEBUG_SERIAL,c);UART_putc(DEBUG_SERIAL,i+48);}
   if (c!=data[i]) {if (i>0) DISP3("*",c,"*");return FALSE;}
    DISP3(".'",c,"'");
   }
 return TRUE;
 }

int main(void)
{  
  Board_init(); // start the board
  setupwatchdoginterrupt();
 // WDT_kick(); // kick the watchdog

  UART_init(DEBUG_SERIAL, 115200);   DISP1("\n\n\r   I2C Satellite I2C Camera test.\n\rWaiting for power on\n\r")
  UART_init(CAM_SERIAL, 115200);   

 // start the console, print bootup message (below)

 UART_putb_nonblocking(CAM_SERIAL,LK_RESET,sizeof(LK_RESET)); // tell it to reset 

   while(1) // infinite loop
 {
 unsigned char c=0;
 for (unsigned int wdt_kicker=100000;wdt_kicker>0;wdt_kicker--) // repeat this to kick wdt at correct time.
  {    
     
    // magnetometer should be in power down mode with data ready, if not wait for it
    // main loop here
    if (match_string(LK_POWERUP)) DISP1("FOUND POWERUP!");
    }
 // WDT_kick();
 }
}


