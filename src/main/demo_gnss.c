/* firmware.h contains all relevant headers */
#include "../firmware.h"

int main(void){
    Board_init();
    WDT_kick();
    LED_on(LED_B);

    while(1){
        GNSS_Init();
        UART_init(UART_CAMERA, 9600);
        char message[200] = "\0";
        GNSS_getData(message);
        UART_puts(UART_CAMERA, message);
        Delay_ms(2000);
    }
    return 0;
}