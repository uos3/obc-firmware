
// gps test
//
// Suzanna Lucarotti (c) 01/09/2017
//
// for use with the UoS3 Cubesat
//
// current initial aim is to display coms messages from GPS chip to serial, problems locking and NMEA decoding a step further
// 
// not finding the UART layer suitable for detailed reading from gps, so rewriting uart base layer

#include "../firmware.h"

#include "inc/hw_types.h"

#include <stdlib.h>
#include <stdarg.h>

#include "../board/uart.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

#define DEBUG_SERIAL UART_CAMERA // this is the serial port debug messages will come through (UART_CAMERA or UART_GNSS)
#define GPS_SERIAL UART_GNSS
#define CAM_SERIAL UART_CAMERA

#define DEBUG_SERIAL CAM_SERIAL

 //////////////////////////////////////////////////////////////////
 // the actual main code
 //////////////////////////////////////////////////////////////////

int main(void)
{  
  Board_init(); // start the board
 setupwatchdoginterrupt();

  UART_init(DEBUG_SERIAL,115200);   DISP1("\n\n\r    Satellite GPS test.\n")
  UART_init(GPS_SERIAL,9600); // initial starting baud for gps

 // start the console, print bootup message (below)
 
   while(1) // infinite loop
 {   
     
    UART_puts(GPS_SERIAL,"fix position\n\r");
  
    while (UART_charsAvail(GPS_SERIAL))
    {
   char c=UART_getc(GPS_SERIAL);
   UART_putc_nonblocking(DEBUG_SERIAL,c);
    }
  }
}