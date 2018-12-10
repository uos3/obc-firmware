/* firmware.h contains all relevant headers */
#include "../firmware.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


/*int main(void){
    Board_init();
    WDT_kick();
    LED_on(LED_B);

    while(1){
        GNSS_Init();
        UART_init(UART_CAMERA, 9600);
        char message[500] = "\0";
        GNSS_getData(message);
        UART_puts(UART_CAMERA, message);
        Delay_ms(10000);

    }
    return 0;
}*/

int main(void){
    Board_init();
    WDT_kick();
    LED_on(LED_B);
    int32_t *longitude, *latitude, *altitude;
    uint32_t *long_sd, *lat_sd, *alt_sd;

    while(1){
        Delay_ms(2500);
        LED_on(LED_B);
        GNSS_Init();
        UART_init(UART_CAMERA, 9600);
        UART_puts(UART_CAMERA, "\n\rtest\n\r");
        char message[500];
        GNSS_getData(longitude, latitude, altitude, long_sd, lat_sd, alt_sd);
        sprintf(message, "\n\rLongitude: %ld\n\r Latitude: %ld\n\r Altitude: %ld\n\r", (long)longitude, (long)latitude, (long)altitude);
        UART_puts(UART_CAMERA, message);
        Delay_ms(2500);
        LED_off(LED_B);

    }
    return 0;
}