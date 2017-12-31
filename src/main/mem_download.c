
// mem_download
//
// Suzanna Lucarotti (c) 13/9/2017
//
// for use with the UoS3 Cubesat
//
// tests downloading the various memory on the chip for SEE/Camera image/data downloading


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

#define DEBUG_SERIAL GPS_SERIAL

 // the useful hardware constants (only used here so not moved away as yet)

 //////////////////////////////////////////////////////////////////
 // the actual main code
 //////////////////////////////////////////////////////////////////

unsigned int UART_getw(long int serial)
 {
 char c1=UART_getc(serial),c2=UART_getc(serial);
 return c1|(c2<<8);
 }

unsigned int UART_getw4(long int serial)
 {
 char c1=UART_getc(serial),c2=UART_getc(serial),c3=UART_getc(serial),c4=UART_getc(serial);
 return c1|(c2<<8)|(c3<<16)|(c4<<24);
 }


void packet_send (char *start_addr, unsigned int len)
 {
 // send byte data from a given address and send down serial line DEBUG_SERIAL
 // will hang processor if address wrong
 // lets put a CRC check on the packet
 // for CRC gonna use xor initially for speed of implementation

 UART_putc(DEBUG_SERIAL,'@');
 
 //start_addr=0;len=8;

 unsigned char x=0;
 for (char *addr=start_addr;addr<start_addr+len;addr++)
   {
   unsigned int s_addr= (int) start_addr;
   // UART_putc(DEBUG_SERIAL,s_addr&255);
   // UART_putc(DEBUG_SERIAL,s_addr>>8);
   // UART_putc(DEBUG_SERIAL,len&255);
   // UART_putc(DEBUG_SERIAL,len>>8);


    char c=*addr;                     // this may make the compiler scream, but we need to see it
    UART_putc(DEBUG_SERIAL,c);
    x=x ^ c;
   }
   UART_putc(DEBUG_SERIAL,'@');
   UART_putc(DEBUG_SERIAL,x); // write the check byte 

 }


int main(void)
{  
  Board_init(); // start the board
  WDT_kick(); // kick the watchdog 

// UART at 115200, unreliable at 9600

  UART_init(DEBUG_SERIAL, 115200);  UART_puts(DEBUG_SERIAL,"\n\rMEM_DEBUG\n\r");

   while(1) // infinite loop
 {
/* char c;
 char *start_addr=0x4000;
 char *end_addr=start_addr+256;
 for (char *addr=start_addr;addr<end_addr;addr++)
   {
    c=*addr;                     // this may make the compiler scream, but we need to see it
    UART_putc(DEBUG_SERIAL,c);
   }
 UART_puts(DEBUG_SERIAL,"\n\rThat was that\n\r");
 */
 for (unsigned int wdt_kicker=100000;wdt_kicker>0;wdt_kicker--) // repeat this to kick wdt at correct time.
  {    
   char c=0; // echo back to sending port
   if (UART_getc_nonblocking(DEBUG_SERIAL,&c))
      {
       if (c=='@') // message from requestor for address dump
         {
    //      UART_puts(DEBUG_SERIAL,"\r\nreceived request for address\n\r");
          char *ad=UART_getw4(DEBUG_SERIAL);
          unsigned int len=UART_getw(DEBUG_SERIAL);
      //    UART_putstr(DEBUG_SERIAL,"\r\nrequested address : ",ad," requested length :");
     //     UART_putstr(DEBUG_SERIAL,NULL,len,"\r\n");
          packet_send(ad,len);
         }
     //  else
     //     UART_putc(DEBUG_SERIAL,'.');
      }  
 }
  WDT_kick();
 }
}


