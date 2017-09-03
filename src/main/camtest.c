
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

#define DEBUG_SERIAL UART_CAM_HEADER // this is the serial port debug messages will come through (UART_CAM_HEADER or UART_PC104_HEADER)
#define GPS_SERIAL UART_PC104_HEADER
#define CAM_SERIAL UART_CAM_HEADER

#define DEBUG_SERIAL CAM_SERIAL

 
// Wrappers for printing to serial port - always to the one specified as output, placed here close to code for clarity

#define DISP3(x,y,z) UART_putstr(DEBUG_SERIAL,x,y,z); // x and z are strings, y is a number shown as signed base10 16bit
#define DISP2(y,z) UART_putstr(DEBUG_SERIAL,NULL,y,z); 
#define DISP1(x) UART_puts(DEBUG_SERIAL,x);

 //////////////////////////////////////////////////////////////////
 // the actual main code
 //////////////////////////////////////////////////////////////////

int main(void)
{  
  Board_init(); // start the board
  WDT_kick(); // kick the watchdog

  UART_init(DEBUG_SERIAL, 9600);   DISP1("\n\n\r   I2C Satellite I2C Camera test.\n")

 // start the console, print bootup message (below)

 
   while(1) // infinite loop
 {

 for (unsigned int wdt_kicker=wdt_start;wdt_kicker>0;wdt_kicker--) // repeat this to kick wdt at correct time.
  {    
     
    // magnetometer should be in power down mode with data ready, if not wait for it
    // main loop here
  }
  WDT_kick();
 }
}


