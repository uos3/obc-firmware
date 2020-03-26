/**
 * File purpose:        GNSS functionality demo (with time)
 * Last modification:   22/08/2019
 * Status:              Ready for the test
 */

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
    char message[500];

    Board_init();
    watchdog_update = 0xFF;
    enable_watchdog_kick();

    GNSS_Init();                        //initialise GNSS UART line
    UART_init(UART_INTERFACE, 9600);
    UART_puts(UART_INTERFACE, "\n\r>>>>>>>> GNSS DEMO\n\r");

    //To ensure the GNSS has time to boot up
    Delay_ms(5000);
    //IMPLEMENT RETURN 1 LOGIC -- ALL ZEROS
    while(1){
        LED_on(LED_B);

        UART_puts(UART_INTERFACE, "\n\r>>> Taking measurement\n\r");
        GNSS_getData_Timed(&longitude, &latitude, &altitude, &long_sd, &lat_sd, &alt_sd, &week, &seconds, &ex_time);
        UART_puts(UART_INTERFACE, ">>> Measurement collection complete\r\n");
        sprintf(message, "\n\r>>> Longitude: %" PRId32 "\n\r>>> Latitude: %" PRId32 "\n\r>>> Altitude: %" PRId32 "\n\r>>> Latitude s_d: %" PRId8
        "\n\r>>> Longitude s_d: %" PRId8 "\n\r>>> Altitude s_d: %" PRId8 "\n\r>>> Time to execute: %" PRId64 " milliseconds \n\r>>> Week Number: %" PRId16 "\n\r>>> Week Seconds: %" PRId32 "\n\r",
        longitude, latitude, altitude, long_sd, lat_sd, alt_sd, ex_time, week, seconds);
        UART_puts(UART_INTERFACE, message);
        UART_puts(UART_INTERFACE, ">>> Waiting for next collection...\n\r");

        LED_off(LED_B);

        Delay_ms(20000);
        watchdog_update = 0xFF;
    }
    return 0;
}
