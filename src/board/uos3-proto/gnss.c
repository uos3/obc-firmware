#include "../gnss.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"
#include <math.h>
#include "../uart.h"
#include "../rtc.h"
#include <stdint.h>


void GNSS_Init(void){
    UART_init(UART_GNSS, 9600);
}

/*bool GNSS_getData(char *Long, char *Lat, char *Timechar *received_Message){

    RTC_init();

    uint64_t start_timestamp;
    uint64_t timeout = 10000;

    char received_Message[200] = '\0';

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

int GNSS_getData(int32_t *longitude, int32_t *latitude, int32_t *altitude, uint8_t *long_sd, uint8_t *lat_sd, uint8_t *alt_sd, uint32_t *time, uint64_t *ex_time){

    RTC_init();

    uint64_t start_timestamp;
    uint64_t current_time;
    uint64_t timeout = 10000;
    int var_counter = 1;

    char received_Message[200] = "\0";
    char long_str[20] = "\0";
    char lat_str[20] = "\0";
    char alt_str[20] = "\0";
    char long_sd_str[20] = "\0";
    char lat_sd_str[20] = "\0";
    char alt_sd_str[20] = "\0";
    char week_no[20] = "\0";
    char seconds[20] = "\0";
    uint32_t longlat_factor = 7, alt_factor = 2;

    RTC_getTime_ms(&start_timestamp);
    //Abbreviated ascii format gives response easier to pass through algorithm, despite no checksum
    UART_puts(UART_GNSS, "\r\nLOG BESTPOSA ONCE\r\n");

    bool command_wait = true;

    while  (!RTC_timerElapsed_ms(start_timestamp, timeout)){
        
        char c = UART_getc(UART_GNSS);
        //UART_putc(UART_CAMERA, c);
        if(c == ';'){
            command_wait = false;
            var_counter = 1;
        }
        if(command_wait == true){

            if(c==','){
                var_counter++;
                
            }
            if(var_counter==6){
                append(week_no, c);
                //UART_puts(UART_CAMERA, "\r\nWeek no\r\n");
                //UART_puts(UART_CAMERA, c);
            }
            if(var_counter==7){
                append(seconds, c);
            }
    
        }
        if(command_wait == false){
            append(received_Message, c);
            if (c == '*'){
                //Shouldn't be able to get stuck in this loop, always a '\0' character 
                int i = 0;
                while(received_Message[i] != '\0'){
                    if(received_Message[i] == ','){
                        var_counter++;
                        i++;
                        continue;
                    }
                    switch(var_counter){
                        case 3 :
                            //UART_puts(UART_CAMERA, "\n\rlong\n\r");
                            append(long_str, received_Message[i]);
                            break;
                        case 4 :
                            //UART_puts(UART_CAMERA, "\n\rlat\n\r");
                            append(lat_str, received_Message[i]);
                            break;
                        case 5 :
                            //UART_puts(UART_CAMERA, "\n\ralt\n\r");
                            append(alt_str, received_Message[i]);
                            break;
                        case 8 :
                            //UART_puts(UART_CAMERA, "\n\rlongsd\n\r");
                            append(long_sd_str, received_Message[i]);
                            break;
                        case 9 :
                            //UART_puts(UART_CAMERA, "\n\rlatsd\n\r");
                            append(lat_sd_str, received_Message[i]);
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
                UART_puts(UART_CAMERA, "end of adjusted string values");*/

                

                *longitude = (int32_t)(atoi(long_str));
                *latitude = (int32_t)(atoi(lat_str));
                *altitude = (int32_t)(atoi(alt_str));
                compress(long_sd, long_sd_str);
                compress(lat_sd, lat_sd_str);
                compress(alt_sd, alt_sd_str);
                *time  = (uint32_t)(atoi(week_no) * 604799 + atoi(seconds));
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
    *s_d = (uint8_t)(log(atoi(s_d_str))* 100);
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
