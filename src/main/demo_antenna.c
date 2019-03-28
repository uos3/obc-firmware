#include <stdio.h>
#include "../firmware.h"

#define UART_INTERFACE UART_GNSS
#define BURN_TIME 5000
#define WAIT_TIME 30000

void Antenna_deploy_demo(void);

int main(){
    Board_init();
    UART_init(UART_INTERFACE, 9600);
    char output[30];
    Delay_ms(500);
    UART_puts(UART_INTERFACE, "\r\n**Antenna deployment demo**\r\n");


    Delay_ms(WAIT_TIME);

    LED_on(LED_B);

    sprintf(output, "Burn of %d milliseconds has begun\r\n", BURN_TIME);
    UART_puts(UART_INTERFACE, output);

    Antenna_deploy_demo();

    LED_off(LED_B);

    UART_puts(UART_INTERFACE, "Burn has finished\r\n");

    return 0;
}


void Antenna_deploy_demo(void)
{
  GPIO_set(GPIO_PB5);
  Delay_ms(BURN_TIME);
  GPIO_reset(GPIO_PB5);
}