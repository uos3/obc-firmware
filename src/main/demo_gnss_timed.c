/* firmware.h contains all relevant headers */
#include "../firmware.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


int main(void){

    uint64_t ex_time = 0;
    uint32_t seconds = 0;
    uint16_t week = 0;
    int32_t longitude=0, latitude=0, altitude=0;
    uint8_t long_sd=0, lat_sd=0, alt_sd=0;

    Board_init();
    WDT_kick();
    LED_on(LED_B);

    //To ensure the GNSS has time to boot up
    Delay_ms(5000);
    //IMPLEMENT RETURN 1 LOGIC -- ALL ZEROS
    while(1){
        Delay_ms(2500);
        LED_on(LED_B);
        GNSS_Init();
        UART_init(UART_CAMERA, 9600);
        UART_puts(UART_CAMERA, "\n\rGNSS data collection:\n\r");
        char message[500];
        GNSS_getData_Timed(&longitude, &latitude, &altitude, &long_sd, &lat_sd, &alt_sd, &week, &seconds, &ex_time);
        UART_puts(UART_CAMERA, "\n\rData collection complete:");
        sprintf(message, "\n\rLongitude: %" PRId32 "\n\rLatitude: %" PRId32 "\n\rAltitude: %" PRId32 "\n\rLatitude s_d: %" PRId8 
        "\n\rLongitude s_d: %" PRId8 "\n\rAltitude s_d: %" PRId8 "\n\rTime to execute: %" PRId64 " milliseconds \n\rWeek Number: %" PRId16 "\n\rWeek Seconds: %" PRId32 "\n\r",
        longitude, latitude, altitude, long_sd, lat_sd, alt_sd, ex_time, week, seconds);
        UART_puts(UART_CAMERA, message);
        UART_puts(UART_CAMERA, "Waiting for next collection...\n\r");
        Delay_ms(2500);
        LED_off(LED_B);

    }
    return 0;
}
