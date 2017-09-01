
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

/* UART description struct */
typedef struct UART {
  uint32_t peripheral_gpio; // TI Driver GPIO Peripheral Reference
  uint32_t peripheral_uart; // TI Driver UART Peripheral Reference
  uint32_t base_gpio;       // TI Driver GPIO Base Reference
  uint32_t base_uart;       // TI Driver UART Base Reference
  uint32_t pin_rx_function; // TI Driver UART RX Pin Reference
  uint32_t pin_tx_function; // TI Driver UART TX Pin Reference
  uint8_t  pin_rx;          // TI Driver RX Pin Reference
  uint8_t  pin_tx;          // TI Driver TX Pin Reference
  bool     initialised;
} UART;

/* Array of enabled UARTs */
static UART UART_uarts[2] =  // these are the camera port and the gps port in that order
  {
    { 
      SYSCTL_PERIPH_GPIOA,
      SYSCTL_PERIPH_UART0,
      GPIO_PORTA_BASE,
      UART0_BASE,
      GPIO_PA0_U0RX,
      GPIO_PA1_U0TX,
      GPIO_PIN_0,
      GPIO_PIN_1,
      false
    },
    { 
      SYSCTL_PERIPH_GPIOC,
      SYSCTL_PERIPH_UART3,
      GPIO_PORTC_BASE,
      UART3_BASE,
      GPIO_PC6_U3RX,
      GPIO_PC7_U3TX,
      GPIO_PIN_6,
      GPIO_PIN_7,
      false
    }
  };
#define NUMBER_OF_UARTS  ( sizeof(UART_uarts) / sizeof(UART) )

#define check_uart_num(x, y)  if(x >= NUMBER_OF_UARTS) { return y; }

/** Public Functions */

void UART_init(uint8_t uart_num, uint32_t baudrate)
{
  check_uart_num(uart_num,);
  UART *uart = &UART_uarts[uart_num];

  /* Check UART is initialised */
  if(!uart->initialised)
  {
    /* Initialise UART Peripheral if not already initialised */
    if(!SysCtlPeripheralReady(uart->peripheral_uart))
    {
      SysCtlPeripheralEnable(uart->peripheral_uart);
      while(!SysCtlPeripheralReady(uart->peripheral_uart)) { };
    }

    /* Initialise GPIO Peripheral if not already initialised */
    if(!SysCtlPeripheralReady(uart->peripheral_gpio))
    {
      SysCtlPeripheralEnable(uart->peripheral_gpio);
      while(!SysCtlPeripheralReady(uart->peripheral_gpio)) { };
    }

    GPIOPinConfigure(uart->pin_rx_function);
    GPIOPinConfigure(uart->pin_tx_function);
    GPIOPinTypeUART(uart->base_gpio, uart->pin_rx | uart->pin_tx);

    UARTClockSourceSet(uart->base_uart, UART_CLOCK_SYSTEM);

    uart->initialised = true;
  }

  UARTConfigSetExpClk(uart->base_uart, SysCtlClockGet(), baudrate,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

char UART_getc(uint8_t uart_num)
{
  check_uart_num(uart_num, '\0');
  UART *uart = &UART_uarts[uart_num];

  return (char)UARTCharGet(uart->base_uart);
}

void UART_putc(uint8_t uart_num, char c)
{
  check_uart_num(uart_num,);
  UART *uart = &UART_uarts[uart_num];

  UARTCharPut(uart->base_uart, c);
}

void UART_puts(uint8_t uart_num, char *str)
{
  while(*str != '\0')
  {
    UART_putc(uart_num, *str++);
  }
}

void UART_putb(uint8_t uart_num, char *str, uint32_t len)
{
  while(len--)
  {
    UART_putc(uart_num, *str++);
  }
}

bool UART_busy(uint8_t uart_num)
{
  check_uart_num(uart_num, false);
  UART *uart = &UART_uarts[uart_num];

  return UARTBusy(uart->base_uart);
}

bool UART_charsAvail(uint8_t uart_num)
{
  check_uart_num(uart_num, false);
  UART *uart = &UART_uarts[uart_num];

  return UARTCharsAvail(uart->base_uart);
}

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

#define MAX_UART_LENGTH 20

char uart_buffer[MAX_UART_LENGTH+1];

char *readuartstr(uint32_t uart_num) // read a string, NULL if nothing. dump up to full buffer if overrun
 {
 char i=0; // chars read
  UART *uart = &UART_uarts[uart_num];

   while (UARTCharsAvail(uart->base_uart) && uart_buffer[0]==0) // skip zeros
      {uart_buffer[0]=UARTCharGetNonBlocking(uart);UART_putc(DEBUG_SERIAL,'0');
      }
  
   while (UARTCharsAvail(uart->base_uart) && i<MAX_UART_LENGTH && uart_buffer[i]!=0) // while we have chars in buffer
    {
     uart_buffer[i]=UARTCharGetNonBlocking(uart);
     UART_putc(DEBUG_SERIAL,uart_buffer[i]);
     i++;
     UART_putc(DEBUG_SERIAL,'z');
     }
 if (i==0) {//UART_putc(DEBUG_SERIAL,'b');
            return NULL;} // signal didnt find anything

  UART_putc(DEBUG_SERIAL,'1');
  uart_buffer[i]=0;
    UART_putc(DEBUG_SERIAL,'\r');
    UART_putc(DEBUG_SERIAL,'\n');
    UART_puts(DEBUG_SERIAL,uart_buffer);
 return uart_buffer;
 }

int main(void)
{  
  Board_init(); // start the board
  WDT_kick(); // kick the watchdog

  UART_init(DEBUG_SERIAL,9600);   DISP1("\n\n\r    Satellite GPS test.\n")
  UART_init(GPS_SERIAL,9600);

 // start the console, print bootup message (below)

unsigned int wdt_start=20; // loops before kick, not too long or too short or hardware will reset
 
   while(1) // infinite loop
 {

 for (unsigned int wdt_kicker=wdt_start;wdt_kicker>0;wdt_kicker--) // repeat this to kick wdt at correct time.
  {    
     
   UART_putc(DEBUG_SERIAL,'a');
   UART_puts(GPS_SERIAL,"fix position");
    // magnetometer should be in power down mode with data ready, if not wait for it
    // main loop here
   char *current_read=readuartstr(GPS_SERIAL);
   if (current_read) UART_puts(DEBUG_SERIAL,current_read);
  }
 // WDT_kick();
 }
}


