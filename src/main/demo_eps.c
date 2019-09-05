/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>
#define testGet 1
#define batTest 0
int main(void){
  
  Board_init();
  watchdog_update = 0xFF;           //set the watchdog control variable 
  enable_watchdog_kick();
  EPS_init();
  RTC_init();
  UART_init(UART_INTERFACE, 9600);
  Delay_ms(1); // delay for initialisation to finish for UART

  UART_puts(UART_INTERFACE, "\r\n** INIT COMPLETE **\r\n");

  // if (testGet) {
  //   EPS_setPowerRail(0b111111);
  // }
  char output[100];
  uint16_t batt_v, batt_i, batt_t, data;
  uint8_t railNo = 0;
  uint16_t loopCount = 0;




   while(1){
    watchdog_update = 0xFF;
    loopCount++;
    UART_puts(UART_INTERFACE, "\r\nLooping\r\n");

    if (batTest)  {
      batt_v = 0;
      batt_i = 0;
      batt_t = 0;
      if(EPS_getInfo(&batt_v, EPS_REG_BAT_V)){
        UART_puts(UART_INTERFACE, "\r\nSuccessfuly read voltage\r\n");
      }
      sprintf(output, "Voltage: %d\r\n", batt_v);
      UART_puts(UART_INTERFACE, output);
      // if(!EPS_getInfo(&batt_i, EPS_REG_SW_ON)){UART_puts(UART_INTERFACE, "\r\nCRC not matched");}
      // UART_puts(UART_INTERFACE, "\r\nSuccessfuly read rails before setting");
      // sprintf(output,"Current: %x\r\n", batt_i);
      // UART_puts(UART_INTERFACE, output);
      // if(!EPS_setPowerRail(0x24)){UART_puts(UART_INTERFACE, "\r\nUnsuccessful");}
      // if(!EPS_getInfo(&batt_t, EPS_REG_SW_ON)){UART_puts(UART_INTERFACE, "\r\nCRC not matched");}
      // UART_puts(UART_INTERFACE, "\r\nSuccessfuly read rails after setting");
      // sprintf(output,"Current: %x\r\n", batt_t);
      // UART_puts(UART_INTERFACE, output);
    }


    if (testGet) {

      // //When 20 loops have completed, approx 30s
      // if  (loopCount%20 == 0) {
      //   UART_puts(UART_INTERFACE, "Loop 20, testing resetting");

      //   //Switch off all rails
      //   //EPS_writeRegister(0x33, 0);
      //   //Delay_ms(2000);
      //   //Switch on all rails
      //   //EPS_setPowerRail(0x63);
      //   //Reset battery
      //   //EPS_writeRegister(0x04, 0);
      //   //Delay_ms(2000);
      //   //Reset EPS
      //   //EPS_writeRegister(0x03, 0);
      // }
      
      // //Sets 1 rail to off at a time, for an interval of 500ms excluding the TOBC rail
      // EPS_setPowerRail((0x63&~(1<<railNo))|EPS_PWR_MCU);
      // railNo++;
      // if (railNo>5) {
      //   railNo = 0;
      // }
      // Delay_ms(500);

      //Checking the UART is still connected and both devices switched on
      if (EPS_selfTest()) {
        UART_puts(UART_INTERFACE, "SelfTest success\r\n");
      } else {
        UART_puts(UART_INTERFACE, "SelfTest fail\r\n");
      }
      Delay_ms(2000);
      //EPS_testWrongCRC(0x04);
      //EPS_testPartialPacket(0x04);
      uint8_t i;
      for (i=2; i<35; i++)  {
        Delay_ms(1000);
        if(!EPS_getInfo(&data, i))  {
          sprintf(output,"Register %x unsuccessful\r\n", i);
          UART_puts(UART_INTERFACE, output);
          }
        else {
        sprintf(output,"Register: %x Data: %x\r\n", i, data);
        UART_puts(UART_INTERFACE, output);
        }
      }
      
      //EPS_setPowerRail(0x63);
    } 
  Delay_ms(1000);

  }
}