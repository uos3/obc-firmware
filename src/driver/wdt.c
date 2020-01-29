 /**
 * @ingroup uos3-proto
 * @ingroup wdt
 *
 * @file uos3-proto/wdt.c
 * @brief INTERNAL WDT Driver - uos3-proto board
 *
 * @{
 */

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/watchdog.h"

#include "board.h"
#include "delay.h"
#include "wdt.h"
#include "watchdog_ext.h"


void internal_wdt_handler(void){
    watchdog_update--;
    if(watchdog_update>0){
      WatchdogIntClear(WATCHDOG0_BASE);               
      WatchdogIntClear(WATCHDOG1_BASE);
      #ifdef DEBUG_MODE
        char output_buffer[100];
        sprintf(output_buffer, "\r\n$$$ Internal Watchdog cleared - \"watchdog_update\" is: %d - greater than zero", watchdog_update);
        UART_puts(UART_INTERFACE, output_buffer);
      #endif
    }
    #ifdef DEBUG_MODE
      else{   
        char output_buffer2[110]; 
        sprintf(output_buffer2, "\r\n$$$ Internal Watchdog not cleared - \"watchdog_update\" is: %d - smaller or equal to 0\r\n$$$ MCU will reboot", watchdog_update); //only for test
        UART_puts(UART_INTERFACE, output_buffer2);
      }
    #endif
}

void setup_internal_watchdogs(void){
  //watchdog0
  //if(WatchdogRunning(WATCHDOG0_BASE)){return;}              //check if the wdt0 is running, to delete: causing program to hang up
  SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);                //enable wdt0 peripheral
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG0));         //wait to be ready
  //Load watchdog with max possible value 2^32 -> gives int. every ~107s
  WatchdogReloadSet(WATCHDOG0_BASE, 0xFFFFFFFF);        
  WatchdogResetEnable(WATCHDOG0_BASE);                        //enable watchdog to reset the system
  WatchdogIntTypeSet(WATCHDOG0_BASE, WATCHDOG_INT_TYPE_NMI);  //set  watchdog int. as NMI
  //WatchdogStallEnable(WATCHDOG0_BASE);                       //allows the wdt to stop counting when the processor is stopped by the debugger
  //watchdog1 - becauses uses different clock, need to wait for being ready after each write; run approx. on 16Hz clock
  //if(WatchdogRunning(WATCHDOG1_BASE)){return;}              //check if the wdt1 is running
  SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG1);                //enable wdt1 peripheral
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG1));         //wait to be ready
  //Load watchdog with max possible value 2^32 -> gives int. every ~107s
  WatchdogReloadSet(WATCHDOG1_BASE, 0xFFFFFFFF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG1));         //wait to be ready        
  WatchdogResetEnable(WATCHDOG1_BASE);                        //enable watchdog to reset the system
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG1));         //wait to be ready
  WatchdogIntTypeSet(WATCHDOG1_BASE, WATCHDOG_INT_TYPE_NMI);  //set  watchdog int. as NMI
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG1));         //wait to be ready
  //WatchdogStallEnable(WATCHDOG1_BASE);                      //allows the wdt to stop counting when the processor is stopped by the debugger

  IntRegister(FAULT_NMI,internal_wdt_handler);                //set watchdogs interrupt as non-maskable
  IntEnable(FAULT_NMI);                                       //enable it

  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG1));         //wait to be ready
  WatchdogIntEnable(WATCHDOG0_BASE);                          //enables wdt0 interrupt and start timer
  WatchdogIntEnable(WATCHDOG1_BASE);                          //enables wdt1 interrupt and start timer
}
/**
 * @}
 */