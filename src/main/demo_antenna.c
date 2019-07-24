#include <stdio.h>
#include "../firmware.h"

#define BURN_TIME 7140
#define WAIT_TIME 10000

void Antenna_deploy_demo(void);

int main(){
    Board_init();
    RTC_init();
    update_watchdog_timestamp();
    enable_watchdog_kick();
    UART_init(UART_INTERFACE, 9600);
    UART_puts(UART_INTERFACE, "\r\n");
    UART_puts(UART_INTERFACE, "Board and UART Interface Initialised\r\n");
    char output[50];
    uint64_t timestamp_before, timestamp_after;
    Delay_ms(500);
    UART_puts(UART_INTERFACE, "\r\nADM test started... Waiting 30 seconds...\n");
    UART_puts(UART_INTERFACE, "\r\nLED_A will turn on when resistor is initiated, then off when the test is over\r\n");

    for(uint16_t i=0; i<WAIT_TIME/1000; i++)
    {
      sprintf(output, "%d\t", WAIT_TIME/1000 - i);
      UART_puts(UART_INTERFACE, output);
      Delay_ms(1000);
      if(i%5==4) UART_puts(UART_INTERFACE, "\r\n");
    }
    //Delay_ms(WAIT_TIME);
    LED_on(LED_B);
    sprintf(output, "\r\nBurn of %d milliseconds has begun\r\n", BURN_TIME);
    UART_puts(UART_INTERFACE, output);
    RTC_getTime_ms(&timestamp_before);
    sprintf(output, "\r\nSystem time before burning is %lu ms", timestamp_before);
    UART_puts(UART_INTERFACE, output);

    Antenna_deploy_demo();      //call deploy function

    LED_off(LED_B);
    UART_puts(UART_INTERFACE, "\r\nBurn has finished\r\n");
    RTC_getTime_ms(&timestamp_after);
    sprintf(output, "\r\nSystem time after burning is %lu ms\r\n", timestamp_after);
    UART_puts(UART_INTERFACE, output);

    uint64_t difference = timestamp_after - timestamp_before;
    sprintf(output, "\r\nThe burning time was %lu ms total\r\n", difference);
    UART_puts(UART_INTERFACE, output);
    
    while(1){
      update_watchdog_timestamp();
    }
}


void Antenna_deploy_demo(void)
{
  GPIO_set(GPIO_PB0);
  Delay_ms(BURN_TIME);
  GPIO_reset(GPIO_PB0);
}