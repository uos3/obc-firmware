
#include <string.h>
#include <stdio.h>

#include "../driver/board.h"
#include "../driver/wdt.h"
#include "../driver/watchdog_ext.h"
#include "../driver/delay.h"
#include "../component/led.h"

#include "../driver/uart.h"

int main(){
    Board_init();
    UART_init(UART_DEBUG_4, 115200);
    enable_watchdog_kick();
    char output[32];
    int counter = 0;


    LED_on(LED_A);
    for(counter = 0; counter< 2000; counter++){
        sprintf(output, "this is line %d", counter);
        UART_puts(UART_DEBUG_4, output);

    }
    LED_off(LED_B);
    LED_off(LED_A);
}
