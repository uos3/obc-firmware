#include "../gnss.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"
#include "../uart.h"
#include "../rtc.h"
#include <stdint.h>
#define CRC32_POLYNOMIAL 0xEDB88320L
unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */unsigned char *ucBuffer );


void GNSS_Init(void){
    UART_init(UART_GNSS, 9600);
}

/*bool GNSS_getData(/*char *Long, char *Lat, char *Timechar *received_Message){

    RTC_init();

    uint64_t start_timestamp;
    uint64_t timeout = 10000;

    //char received_Message[200] = '\0';

    RTC_getTime_ms(&start_timestamp);

    UART_puts(UART_GNSS, "\r\n#LOG,COM1,0,91.0,UNKNOWN,0,51.872,004c0000,c00c,13307;BESTPOS*a1c4d2a6\r\n");

    bool command_wait = true;

    while  (!RTC_timerElapsed_ms(start_timestamp, timeout)){
        char c = UART_getc(UART_GNSS);
        if(c == '<'){
          command_wait = false;
        }
        if(command_wait == false){
            append(received_Message, c);
            if (c == ']'){

                return true;
            }
        }
    }
  return false;
}*/

bool GNSS_getData(int32_t *longitude, int32_t *latitude, int32_t *altitude, uint32_t *long_sd, uint32_t *lat_sd, uint32_t *alt_sd){

    RTC_init();

    uint64_t start_timestamp;
    uint64_t timeout = 10000;
    int var_counter = 1;

    char received_Message[200] = "\0";
    char long_str[20] = "\0";
    char lat_str[20] = "\0";
    char alt_str[20] = "\0";
    char long_sd_str[20] = "\0";
    char lat_sd_str[20] = "\0";
    char alt_sd_str[20] = "\0";
    uint32_t longlat_factor = 7, alt_factor = 2;

    RTC_getTime_ms(&start_timestamp);
    //Abbreviated ascii format gives response easier to pass through algorithm, despite no checksum
    UART_puts(UART_GNSS, "\r\nLOG BESTPOSA ONCE\r\n");

    bool command_wait = true;

    while  (!RTC_timerElapsed_ms(start_timestamp, timeout)){
        char c = UART_getc(UART_GNSS);
        if(c == ';'){
            command_wait = false;
        }
        if(command_wait == false){
            append(received_Message, c);
            //UART_puts(UART_CAMERA, "Test_1\n\r");
            //UART_putc(UART_CAMERA, c);
            if (c == '*'){
                //Shouldn't be able to get stuck in this loop, always a '\0' character 
                int i = 0;
                UART_puts(UART_CAMERA, "end_2\n\r");
                while(received_Message[i] != '\0'){
                    //UART_puts(UART_CAMERA, "appended");
                    if(received_Message[i] == ','){
                        var_counter++;
                        i++;
                        continue;
                        //UART_puts(UART_CAMERA, "appended");
                    }
                    switch(var_counter){
                        case 3 :
                            UART_puts(UART_CAMERA, "\n\rlong\n\r");
                            append(long_str, received_Message[i]);
                            break;
                        case 4 :
                            UART_puts(UART_CAMERA, "\n\rlat\n\r");
                            append(lat_str, received_Message[i]);
                            break;
                        case 5 :
                            UART_puts(UART_CAMERA, "\n\ralt\n\r");
                            append(alt_str, received_Message[i]);
                            break;
                        case 8 :
                            UART_puts(UART_CAMERA, "\n\rlongsd\n\r");
                            append(long_sd_str, received_Message[i]);
                            break;
                        case 9 :
                            UART_puts(UART_CAMERA, "\n\rlatsd\n\r");
                            append(lat_sd_str, received_Message[i]);
                            break;
                        case 10 :
                            UART_puts(UART_CAMERA, "\n\raltsd\n\r");
                            append(alt_sd_str, received_Message[i]);
                            break;
                        default :
                            break;
                        //UART_puts(UART_CAMERA, "appended");
                    }
                    i++;
                }
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, long_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, lat_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, alt_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, long_sd_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, lat_sd_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, alt_sd_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, "end of string values");

                adjust_decimal(longlat_factor, long_str);
                adjust_decimal(longlat_factor, lat_str);
                adjust_decimal(alt_factor , alt_str);

                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, long_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, lat_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, alt_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, long_sd_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, lat_sd_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, alt_sd_str);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, "end of adjusted string values");

                longitude = (int32_t)(atoi(long_str));
                latitude = (int32_t)(atoi(lat_str));
                altitude = (int32_t)(atoi(alt_str));
                long_sd = (uint32_t)(atoi(long_sd_str));
                lat_sd = (uint32_t)(atoi(lat_sd_str));
                alt_sd = (uint32_t)(atoi(alt_sd_str));
                return true;
            }
        }
    }
  return false;
}


/*bool GNSS_sendCommand(char GNSS_Command, char *GNSS_reply){
    return false;
}*/



/*bool GNSS_config(char *receiver_config){
    RTC_init();

    uint64_t start_timestamp;
    uint64_t timeout = 10000;

    //char received_Message[200] = '\0';

    RTC_getTime_ms(&start_timestamp);

    UART_puts(UART_GNSS, "\r\n#LOG,COM1,0,91.0,UNKNOWN,0,51.872,004c0000,c00c,13307;RXCONFIG*b6c29fe9\r\n");

    bool command_wait = true;

    while  (!RTC_timerElapsed_ms(start_timestamp, timeout)){
        char c = UART_getc(UART_GNSS);
        if(c == '\r'){
          command_wait = false;
        }
        if(command_wait == false){
            append(receiver_config, c);
            if (c == ']'){
                return true;
            }
        }
    }
  return false;
}*/

void append(char *append_str, char append_c){
    int len = strlen(append_str);
    append_str[len] = append_c;
    append_str[len+1] = '\0';
}

void adjust_decimal(uint32_t factor, char *to_adjust){
    uint32_t adjustment_counter = 0, decimal_counter = 0, len_counter =0;
    while(to_adjust[len_counter] != '\0'){
        len_counter++;
    }
    for(decimal_counter = 0; decimal_counter < len_counter; decimal_counter++){
        if(to_adjust[decimal_counter] == '.'){
            if((decimal_counter + factor) > len_counter){return;}
            for(adjustment_counter = decimal_counter; adjustment_counter <= (decimal_counter + factor) ; adjustment_counter++){
                to_adjust[adjustment_counter] = to_adjust[adjustment_counter + 1];
                if(adjustment_counter == (decimal_counter + factor)){
                    to_adjust[adjustment_counter + 1] = '.';
                    return;
                }
            }
        }
    }
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