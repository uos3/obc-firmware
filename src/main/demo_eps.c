/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>


int main(void){
  Board_init();
  EPS_init();
  RTC_init();
  UART_init(UART_INTERFACE, 9600);
  Delay_ms(1); // delay for initialisation to finish for UART

  UART_puts(UART_INTERFACE, "\r\n** INIT COMPLETE **\r\n");

  char output[100];
  uint16_t batt_v, batt_i, batt_t;

  //
  // EPS_getBatteryInfo(&batt_i, EPS_REG_BAT_I);
  // sprintf(output,"Current: %+06d\r\n", batt_i);
  // UART_puts(UART_INTERFACE, output);
  
  /* EPS_getBatteryInfo(&batt_t, EP`S_REG_BAT_T);
   sprintf(output,"Batt Temperature: %+06d\r\n", batt_t);
   UART_puts(UART_INTERFACE, output);*/

   uint16_t eps_field;


   while(1){/*
     LED_toggle(LED_B);
	 uint8_t i;
	 for (i = 2; i < 0x35; i++) {
		EPS_getInfo(&eps_field, i);
		sprintf(output, "RegID: %x		Data: %u\r\n", i, eps_field);
		UART_puts(UART_INTERFACE, output);
    Delay_ms(1000);
   }*/

    batt_v = 0;
    batt_i = 0;
    EPS_getInfo(&batt_v, 0x50);
    UART_puts(UART_INTERFACE, "\r\nSuccessfuly read voltage\r\n");
    sprintf(output,"Voltage: %+06d\r\n", batt_v);
    UART_puts(UART_INTERFACE, output);
    Delay_ms(5000);/*
    EPS_getInfo(&batt_i, EPS_REG_BAT_I);
    UART_puts(UART_INTERFACE, "\r\nSuccessfuly read current\r\n");
    sprintf(output,"Current: %+06d\r\n", batt_i);
    UART_puts(UART_INTERFACE, output);
    //*/
     //Delay_ms(5000);
   }
}