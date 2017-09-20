
// UART passthrough code
//
// Suzanna Lucarotti (c) 11/8/2017
//
// for use with the UoS3 Cubesat
//
//provides debugging of hex numbers coming back from port, with serial in
// when connected through a terminal


#include "../firmware.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include <stdlib.h>
#include <stdarg.h>

#define GPS_SERIAL UART_PC104_HEADER
#define CAM_SERIAL UART_CAM_HEADER

#define DEBUG_PORT GPS_SERIAL
#define NORMAL_PORT CAM_SERIAL

 // the useful hardware constants (only used here so not moved away as yet)
 
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

// UART at 115200, unreliable at 9600

  UART_init(CAM_SERIAL, 115200);  UART_puts(CAM_SERIAL,"\nreset\n\r");
  UART_init(GPS_SERIAL, 115200);  UART_puts(GPS_SERIAL,"\nreset\n\r");

   while(1) // infinite loop
 {
 char buffer[4];

 for (unsigned int wdt_kicker=100000;wdt_kicker>0;wdt_kicker--) // repeat this to kick wdt at correct time.
  {    
    char c; // echo back to sending port
    if (UART_getc_nonblocking(DEBUG_PORT,&c)) {UART_putc_nonblocking(NORMAL_PORT,c); } // input
    if (UART_getc_nonblocking(NORMAL_PORT,&c)) {//UART_putc_nonblocking(GPS_SERIAL,c);
                 int w=snprintf(buffer,4,"%x",c); //itoa(c,buffer,16);
                 if (w==1) {buffer[1]=buffer[0];buffer[1]='0';}
                 UART_putc_nonblocking(DEBUG_PORT,'.');
        //         UART_putc_nonblocking(DEBUG_PORT,'[');
                 UART_putc_nonblocking(DEBUG_PORT,buffer[0]);
                 UART_putc_nonblocking(DEBUG_PORT,buffer[1]);
                // UART_putb(DEBUG_PORT,buffer,2); // using this instead slows it down, not slowing down means odd behaviour, unmatched brackets etc
          //       UART_putc_nonblocking(DEBUG_PORT,']');  
       } //output
  
   }
  WDT_kick();
 }
}


