/**
 * File purpose:        Antenna deployment demo
 * Last modification:   22/08/2019
 * Status:              Redundant
 */
#include <stdio.h>
#include "../firmware.h"

#define UART_INTERFACE UART_GNSS
#define BURN_TIME 7000
#define WAIT_TIME 30000

void Antenna_deploy_demo(void);

int main(){

    Board_init();
    WDT_kick();
    uint64_t start_timestamp;
    uint64_t timeout = WAIT_TIME;
    RTC_getTime_ms(&start_timestamp);
    RTC_init();
    UART_init(UART_INTERFACE, 9600);
    char output[30];

    Delay_ms(500);

    UART_puts(UART_INTERFACE, "\r\n**Antenna deployment demo**\r\n");

    while(!RTC_timerElapsed_ms(start_timestamp, timeout)){
        Delay_ms(500);
        WDT_kick();
    }

    LED_on(LED_B);
    sprintf(output, "Burn of %d milliseconds has begun\r\n", BURN_TIME);
    UART_puts(UART_INTERFACE, output);

    Antenna_deploy_demo();

    LED_off(LED_B);
    UART_puts(UART_INTERFACE, "Burn has finished\r\n");

    while(1){
        Delay_ms(500);
        WDT_kick();
    }

    return 0;
}


void Antenna_deploy_demo(void)
{
    uint64_t start_timestamp;
    uint64_t timeout = BURN_TIME;
    RTC_getTime_ms(&start_timestamp);
    GPIO_set(GPIO_PB5);
    while(!RTC_timerElapsed_ms(start_timestamp, timeout)){
        Delay_ms(200);
        WDT_kick();

    }
    //Delay_ms(BURN_TIME);
    GPIO_reset(GPIO_PB5);
}