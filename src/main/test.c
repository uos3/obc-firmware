// #include <stdio.h>
// #include <stdint.h>
// #include <string.h>
#include "../firmware.h"


int main(void){
    while(1){
        Board_init();
        UART_init(UART_CAMERA, 9600);
        WDT_kick();
        LED_on(LED_B);
        int32_t *longitude,  *latitude,  *altitude;
        //char output[] = "\0";
        //adjust_decimal(20, output);
        char message[100];
        char long_str[20] = "0.00000000000";
        char lat_str[20] = "0.00000000000";
        char alt_str[20] = "0.00000000000";

        uint32_t longlat_factor = 7, alt_factor = 2;

        adjust_decimal(longlat_factor, long_str);
        adjust_decimal(longlat_factor, lat_str);
        adjust_decimal(alt_factor , alt_str);

        longitude = (int32_t)(atoi(long_str));
        latitude = (int32_t)(atoi(lat_str));
        altitude = (int32_t)(atoi(alt_str));

        sprintf(message, "%ld\n%ld\n%ld\n\r", longitude, latitude, altitude);
        UART_puts(UART_CAMERA, message);
        Delay_ms(5000);
    }
    return 0;
}
