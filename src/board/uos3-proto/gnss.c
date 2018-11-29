#include "../gnss.h"
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "../uart.h"
#include "../rtc.h"
#define CRC32_POLYNOMIAL 0xEDB88320L
unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */unsigned char *ucBuffer );


void GNSS_Init(void){
    UART_init(UART_GNSS, 9600);
}

bool GNSS_getData(/*char *Long, char *Lat, char *Time*/char *received_Message){

    RTC_init();

    uint64_t start_timestamp;
    uint64_t timeout = 10000;

    //char received_Message[200] = '\0';

    RTC_getTime_ms(&start_timestamp);

    UART_puts(UART_GNSS, "\r\n#LOG,COM1,0,91.0,UNKNOWN,0,51.872,004c0000,c00c,13307;GPGLL*b9e75c75\r\n");

    bool command_wait = true;

    while  (!RTC_timerElapsed_ms(start_timestamp, timeout)){
        char c = UART_getc(UART_GNSS);
        if(c == '\r'){
          command_wait = false;
        }
        if(command_wait == false){
            append(received_Message, c);
            if (c == '\r'){
                return true;
            }
        }
    }
  return false;
}

bool GNSS_sendCommand(char GNSS_Command, char *GNSS_reply){
    return false;
}

void append(char *append_str, char append_c){
    int len = strlen(append_str);
    append_str[len] = append_c;
    append_str[len+1] = '\0';
}

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */unsigned char *ucBuffer ) /* Data block */{
  unsigned long ulTemp1;
  unsigned long ulTemp2;
  unsigned long ulCRC = 0;
  while ( ulCount-- != 0 )
  {
    ulTemp1 = ( ulCRC >> 8 ) & 0x00FFFFFFL;
    ulTemp2 = CRC32Value( ((int) ulCRC ^ *ucBuffer++ ) & 0xff );
    ulCRC = ulTemp1 ^ ulTemp2;
  }
  return( ulCRC );
}
//char *i = "LOG,COM1,0,91.0,UNKNOWN,0,51.872,004c0000,c00c,13307;LOGLIST";
//unsigned long iLen = strlen(i);
//unsigned long CRC = CalculateBlockCRC32(iLen, (unsigned char*)i);
//\r\n#LOG,COM1,0,91.0,UNKNOWN,0,51.872,004c0000,c00c,13307;LOGLIST*3b4994ae\r\n