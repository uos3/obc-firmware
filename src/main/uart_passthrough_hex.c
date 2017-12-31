
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

#define GPS_SERIAL UART_GNSS
#define CAM_SERIAL UART_CAMERA

#define DEBUG_PORT GPS_SERIAL
#define NORMAL_PORT CAM_SERIAL

 //////////////////////////////////////////////////////////////////
 // the actual main code
 //////////////////////////////////////////////////////////////////

int main(void)
{  
  Board_init(); // start the board
  setupwatchdoginterrupt();

// UART at 115200, unreliable at 9600

  UART_init(CAM_SERIAL, 115200);  
  UART_init(GPS_SERIAL, 115200);  

  UART_puts(DEBUG_PORT,"\n\rreset\n\r");

   while(1) // infinite loop
 {
 char buffer[4];

    char c; // echo back to sending port
    if (UART_getc_nonblocking(DEBUG_PORT,&c)) {UART_putc_nonblocking(NORMAL_PORT,c); } // input
    if (UART_getc_nonblocking(NORMAL_PORT,&c)) {//UART_putc_nonblocking(GPS_SERIAL,c);
                 int w=snprintf(buffer,4,"%x",c); //itoa(c,buffer,16);
                 if (w==1) {buffer[1]=buffer[0];buffer[1]='0';}
                 UART_putc_nonblocking(DEBUG_PORT,'.');
        //         UART_putc_nonblocking(DEBUG_PORT,'[');
                 UART_putc_nonblocking(DEBUG_PORT,buffer[0]);
                 UART_putc_nonblocking(DEBUG_PORT,buffer[1]);
                 if (c==0) UART_putc_nonblocking(DEBUG_PORT,'\n');
                 if (c==0) UART_putc_nonblocking(DEBUG_PORT,'\r');
                // UART_putb(DEBUG_PORT,buffer,2); // using this instead slows it down, not slowing down means odd behaviour, unmatched brackets etc
          //       UART_putc_nonblocking(DEBUG_PORT,']');  
       } //output
  
  }
}


