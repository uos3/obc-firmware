#include "../driver/board.h"
#include "../driver/delay.h"
#include "../driver/wdt.h"
#include "../driver/uart.h"
#include "../driver/watchdog_ext.h"
#include "../component/led.h"


int main(void){
    Board_init();
    enable_watchdog_kick();
    UART_init(UART_DEBUG_4, 9600);

    UART_puts(UART_DEBUG_4, "Blinkly example with 500ms delay.\r\n");
    watchdog_update= 0xFF;
    while(1){
        UART_puts(UART_DEBUG_4, "Led on\r\n");
        LED_on(LED_A);
        LED_on(LED_B);
        Delay_ms(500);
        UART_puts(UART_DEBUG_4, "Led off\r\n");
        LED_off(LED_A);
        LED_off(LED_B);
        Delay_ms(500);
        watchdog_update= 0xFF;
    }
}