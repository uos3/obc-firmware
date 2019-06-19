#include "../gnss.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"
#include <math.h>
#include "../uart.h"
#include "../rtc.h"
#include <stdint.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

uint64_t start_timestamp;
uint64_t current_time;
uint64_t timeout;

void GNSS_Init(void){
    UART_init(UART_GNSS, 9600);
}

int GNSS_getData(int32_t *longitude, int32_t *latitude, int32_t *altitude, uint8_t *long_sd, uint8_t *lat_sd, uint8_t *alt_sd, uint16_t *week_num, uint32_t *week_seconds, uint64_t *ex_time){

    RTC_init();

    /*uint64_t start_timestamp;
    uint64_t current_time;*/
    timeout = 10000;
    int var_counter = 1;
    bool star_trigger = false;

    char crc_string[200] = "\0";
    char received_Message[200] = "\0";
    char long_str[20] = "\0";
    char lat_str[20] = "\0";
    char alt_str[20] = "\0";
    char long_sd_str[20] = "\0";
    char lat_sd_str[20] = "\0";
    char alt_sd_str[20] = "\0";
    char week_no[20] = "\0";
    char seconds[20] = "\0";
    char crc_str[30] = "\0";
    char crc_hex[30];

    uint32_t longlat_factor = 6, alt_factor = 1, seconds_factor = 2;

    RTC_getTime_ms(&start_timestamp);
    UART_puts(UART_GNSS, "\r\n#LOG,COM1,0,91.0,UNKNOWN,0,51.872,004c0000,c00c,13307;BESTPOSA*3ecfb309\r\n");

    bool command_wait = true;
    uint16_t hash_count = 0;

    while  (!RTC_timerElapsed_ms(start_timestamp, timeout)){
        
        char c = UART_getc(UART_GNSS);
        UART_putc(UART_CAMERA, c);
        if(c =='#' && hash_count < 2){
            hash_count++;
            continue;
        }
        if(hash_count < 2){
            continue;
        }
        if(c == '*'){
            star_trigger = true;
        }
 
        if(hash_count == 2 && star_trigger != true){
            /*if(c=='"'){
                append(crc_string, '\\');
            }*/
            append(crc_string, c);
        }
        if(hash_count == 2 && star_trigger == true){
            if(c != '*'){
                append(crc_str, c);
            }
        }
        if(c == ';'){
            command_wait = false;
            var_counter = 1;
        }
        if(command_wait == true){

            if(c==','){
                var_counter++;
                
            }
            if(var_counter==6 && c != ','){
                append(week_no, c);
                //UART_puts(UART_CAMERA, "\r\nWeek no\r\n");
                //UART_puts(UART_CAMERA, c);
            }
            if(var_counter==7 && c != ','){
                append(seconds, c);
            }
    
        }
        if(command_wait == false){
            append(received_Message, c);
            if (c == '\r'){
                //Shouldn't be able to get stuck in this loop, always a '\0' character 
                int i = 0;
                while(received_Message[i] != '\0'){
                    if(RTC_timerElapsed_ms(start_timestamp, timeout)){return 1;}
                    if(received_Message[i] == ','){
                        var_counter++;
                        i++;
                        continue;
                    }
                    switch(var_counter){
                        case 3 :
                            //UART_puts(UART_CAMERA, "\n\rlong\n\r");
                            append(lat_str, received_Message[i]);
                            break;
                        case 4 :
                            //UART_puts(UART_CAMERA, "\n\rlat\n\r");
                            append(long_str, received_Message[i]);
                            break;
                        case 5 :
                            //UART_puts(UART_CAMERA, "\n\ralt\n\r");
                            append(alt_str, received_Message[i]);
                            break;
                        case 8 :
                            //UART_puts(UART_CAMERA, "\n\rlongsd\n\r");
                            append(lat_sd_str, received_Message[i]);
                            break;
                        case 9 :
                            //UART_puts(UART_CAMERA, "\n\rlatsd\n\r");
                            append(long_sd_str, received_Message[i]);
                            break;
                        case 10 :
                            //UART_puts(UART_CAMERA, "\n\raltsd\n\r");
                            append(alt_sd_str, received_Message[i]);
                            break;
                        default :
                            break;
                    }
                    i++;
                }
                /*UART_puts(UART_CAMERA, "\n\r");
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
                UART_puts(UART_CAMERA, "end of string values");*/

                adjust_decimal(longlat_factor, long_str);
                adjust_decimal(longlat_factor, lat_str);
                adjust_decimal(alt_factor , alt_str);
                adjust_decimal(seconds_factor, seconds);

                /*UART_puts(UART_CAMERA, "\n\r");
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
                UART_puts(UART_CAMERA, seconds);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, "end of adjusted string values\n");*/

                //UART_puts(UART_CAMERA, crc_string);
            

                *longitude = (int32_t)(atoi(long_str));
                *latitude = (int32_t)(atoi(lat_str));
                *altitude = (int32_t)(atoi(alt_str));
                compress(long_sd, long_sd_str);
                compress(lat_sd, lat_sd_str);
                compress(alt_sd, alt_sd_str);
                char buffer[15];
                itoa(lat_sd, buffer, 10);
                UART_puts(UART_CAMERA, "\n\r");
                UART_puts(UART_CAMERA, buffer);
                UART_puts(UART_CAMERA, "\n\r");

                *week_num = (uint16_t)atoi(week_no);
                *week_seconds  = (uint32_t)atoi(seconds);

                unsigned long iLen = strlen(crc_string);
                unsigned long CRC = CalculateBlockCRC32(iLen, (unsigned char*)crc_string);

                sprintf(crc_hex, "%lx", CRC);

                for(int crc_count = 0; crc_count < 8; crc_count++){
                    if(crc_hex[crc_count] != crc_str[crc_count]){
                        *longitude = 0;
                        *latitude = 0;
                        *altitude = 0;
                        *long_sd = 0;
                        *lat_sd = 0;
                        *alt_sd = 0;
                        *week_num = 0;
                        *week_seconds = 0;
                        UART_puts(UART_CAMERA, "\r\n**FAILED!!**\r\n");
                        return 1;
                    }
                }

                UART_puts(UART_CAMERA, "\r\nSUCCESS\r\n");


                UART_puts(UART_CAMERA, crc_hex);
                RTC_getTime_ms(&current_time);
                *ex_time = current_time - start_timestamp;
                return 0; 
            }
        }
    }
  return 1;
}

void append(char *append_str, char append_c){
    size_t len = strlen(append_str);
    append_str[len] = append_c;
    append_str[len+1] = '\0';
}

void compress(uint8_t *s_d, char s_d_str[]){
    double intermed;
    intermed = log10(atof(s_d_str))* 100;
    *s_d = (uint8_t)intermed;
}

void adjust_decimal(uint32_t factor, char *to_adjust){
    uint32_t adjustment_counter = 0, decimal_counter = 0, len_counter =0;
    while(to_adjust[len_counter] != '\0'){
        if(RTC_timerElapsed_ms(start_timestamp, timeout)){return 1;}
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

/*CALCULATES CRC FOR GNSS*/

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */unsigned char *ucBuffer ) /* Data block */
{
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

unsigned long CRC32Value(int i)
{
    int j;
    unsigned long ulCRC;
    ulCRC = i;
    for ( j = 8 ; j > 0; j-- )
    {
        if ( ulCRC & 1 ){
            ulCRC = ( ulCRC >> 1 ) ^ CRC32_POLYNOMIAL;
        }
        else{
            ulCRC >>= 1;
        }
    }
    return ulCRC;
}
