/* firmware.h contains all relevant headers */
#include "../firmware.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


int8_t get_gnss_data(void){

    int32_t *longitude_p, *latitude_p, *altitude_p;
    uint64_t *ex_time_p;
    uint64_t ex_time = 0;
    uint32_t *time_p;
    uint32_t time = 0;
    int32_t longitude=0, latitude=0, altitude=0;
    uint8_t *long_sd_p, *lat_sd_p, *alt_sd_p;
    uint8_t long_sd=0, lat_sd=0, alt_sd=0;

    longitude_p = &longitude;
    latitude_p = &latitude;
    altitude_p = &altitude;
    long_sd_p = &long_sd;
    lat_sd_p = &lat_sd;
    alt_sd_p = &alt_sd;
    ex_time_p = &ex_time;
    time_p = &time;

    Board_init();
    WDT_kick();
    LED_on(LED_B);

    LED_on(LED_B);

    //To ensure the GNSS has time to get a lock
    Delay_ms(5000);

    GNSS_Init();
    UART_init(UART_INTERFACE, 9600);
    UART_puts(UART_INTERFACE, "\n\rGNSS data collection...\n\r");
    char message[500];
    GNSS_getData(longitude_p, latitude_p, altitude_p, long_sd_p, lat_sd_p, alt_sd_p, time_p, ex_time_p);
    sprintf(message, "\n\rLongitude: %" PRId32 "\n\rLatitude: %" PRId32 "\n\rAltitude: %" PRId32 "\n\rLatitude s_d: %" PRId8 
    "\n\rLongitude s_d: %" PRId8 "\n\rAltitude s_d: %" PRId8 "\n\rTime to execute: %" PRId64 " milliseconds \n\rGPS Time: %" PRId32 "\n\r",
    *longitude_p, *latitude_p, *altitude_p, *long_sd_p, *lat_sd_p, *alt_sd_p, *ex_time_p, *time_p);
    UART_puts(UART_INTERFACE, message);
    UART_puts(UART_INTERFACE, "\n\rGNSS data collection complete.\n\r");
    LED_off(LED_B);

    return 0;
}
