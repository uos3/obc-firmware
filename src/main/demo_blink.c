#include "../driver/board.h"
#include "../driver/delay.h"
#include "../driver/wdt.h"
#include "../driver/watchdog_ext.h"
#include "../component/led.h"


int main(void){
    Board_init();
    watchdog_update= 0xFF;
    enable_watchdog_kick();

    while(1){
        LED_on(LED_A);
        LED_on(LED_B);
        Delay_ms(500);
        LED_on(LED_A);
        LED_on(LED_B);
        Delay_ms(500);
        watchdog_update= 0xFF;
    }
}