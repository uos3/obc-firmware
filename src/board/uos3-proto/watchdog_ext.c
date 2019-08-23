/**
 * @ingroup uos3-proto
 * @ingroup watchdog_ext
 *
 * @file uos3-proto/watchdog_ext.c
 * @brief External Watchdog Driver
 * 
 * @ALL UART connected statements are for the purpose of testing only!
 * @{
 */

#include "board.h"
#include "../watchdog_ext.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "../led.h"

typedef struct WDT {
  uint8_t gpio;
} WDT;
static WDT wdt = { GPIO_PF4 };

//following code: Maciej Dudziak
//Timer0 and Timer1 will be used for kicking the ext watchdog; timer0 interrupt will rise the kick singal
//Timer1 interrupt will be triggered 10us after rising edge - beacause of the required 3us hold time - and will lower kick signal to 0

//Timer0 interrupt handler
void Timer0IntHandler(void)
{	
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);		//clear the interrupt mask
	watchdog_update--;
	#ifdef DEBUG_PRINT
	UART_puts(UART_INTERFACE, "\r\n### External Watchdog interrupt Handler");
	#endif
	if(watchdog_update>0){
	GPIO_set(wdt.gpio);									//set the kick signal high
	#ifdef DEBUG_PRINT
	char output_buffer[50];
	sprintf(output_buffer, "\r\n### External watchdog kicked - \"watchdog_update\" is:	%d - greater than zero", watchdog_update);
	UART_puts(UART_INTERFACE, output_buffer);
	LED_on(LED_B);
	#endif
	TimerEnable(TIMER1_BASE, TIMER_A);					//start timer1
	}
	#ifdef DEBUG_PRINT
	else
	{
	char output_buffer2[100];
	sprintf(output_buffer2, "\r\n### External watchdog not kicked!!! - \"watchdog_update\" is:	%d - smaller or equal to 0\r\n### MCU WILL REBOOT!!!", watchdog_update);
	UART_puts(UART_INTERFACE, output_buffer2);
	}
	#endif
}
//Timer1 interrupt handler
void Timer1IntHandler(void)
{
	//UART_puts(UART_GNSS, "In second - lowering - interrupt\r\n");
	TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);		//clear the interrupt
	GPIO_reset(wdt.gpio);								//set the kick signal low
	TimerDisable(TIMER1_BASE, TIMER_A);					//stop the Timer1
}

//setting the timers for the watchdog kick: using timer0 nad timer1; this timer will be responsible for rising edge of the wdt kick
void setinterrupt_wdt_risingedge(void)
{
	unsigned long ulPeriod;											//clock rate of the system; it is the value corresping to 1s of operation/counting down
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);					//turn on WideTimer0
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));			//wait for the timer to be ready
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);	//set half-timer (32bit) to the periodically operation mode, this is wide timer, which consist of two 32-bit timers; we need to use only one 32-bit, so the half
	ulPeriod = ((SysCtlClockGet() / 10) * 18);						//divide the period corresponding to 1s by 10, to have value of timer corresponding to 0.1s; *18 to have a period for 1.8s
	TimerLoadSet(TIMER0_BASE,TIMER_A,ulPeriod-1);
	TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);		//for dynamic interrupt calculation
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);				//enable interrupt
	TimerEnable(TIMER0_BASE, TIMER_A);								//start timer

}

//this is just setting up the timer; timer will be started in the interrupt handler for the rising edge signal;
void setinterrupt_wdt_fallingedge(void){
	unsigned long ulPeriod;		//clock rate of the system, used in period correspond to 1s
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);					//turn on WideTimer1
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));			//wait for the timer to be ready
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);	//set half-timer (32bit) to the periodically operation mode
	ulPeriod = (SysCtlClockGet() / 10000);							//divide the period corresponding to 1s by 10000, to have value of timer correspondit to 10us
	TimerLoadSet(TIMER1_BASE,TIMER_A,ulPeriod-1);
	TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1IntHandler);		//for dynamic interrupt calculation
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);				//enable interrupt
}
//TO BE CALLED IN THE SETUP FUNCTION IN THE MISSION CODE
//function to enable watchdog kick functionality by calling two setup functions;
void enable_watchdog_kick(void)
{
	setinterrupt_wdt_risingedge();
	setinterrupt_wdt_fallingedge();
}
