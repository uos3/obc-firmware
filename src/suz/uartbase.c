// UART base
//
// Suzanna Lucarotti (c) 01/09/2017
//
// for use with the UoS3 Cubesat
//

// the uart interface specific commands, basically helper functions sitting above the hardware, but below specific device code

#include <stdlib.h>

/// Fancy Console code to display without scrolling, provided for initial testing purposes, not for flight.

#define STRING_BUFFER_LENGTH 20 // rough hack for testing

static char string_buffer[STRING_BUFFER_LENGTH];
static char string_buffer2[STRING_BUFFER_LENGTH];

void UART_putnum(unsigned int serialport,signed long x)
 {
  itoa(abs(x),string_buffer,10); // 16 for hex, 10 for decimal
  unsigned int len=strlen(string_buffer);
  unsigned int targetlen=6-len;
  strcpy(string_buffer2,"+00000");
  strcpy(string_buffer2+targetlen,string_buffer);
  if (x<0) string_buffer2[0]='-';
  UART_puts(serialport,string_buffer2);
}

void UART_putstr(unsigned int serialport, char *s1,signed long x, char *s2)
 {
  if (s1!=NULL) UART_puts(serialport,s1);
  UART_putnum(serialport,x);
  if (s2!=NULL) UART_puts(serialport,s2);
 }  

