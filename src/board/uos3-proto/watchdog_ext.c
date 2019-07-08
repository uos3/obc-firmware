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


//#include "../firmware.h"
#include "board.h"
#include "../watchdog_ext.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
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
	//just for testing
	UART_puts(UART_GNSS, "In watchdog interrupt\r\n");
	char output [100];
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);		//clear the interrupt
	RTC_getTime(&temporary_time);						//get the RTC time at this moment
	if((temporary_time-missionloop_time)<120){			//compare it with the last time saved in the mission loop
	GPIO_set(wdt.gpio);									//set the kick signal high
	//just for testing
	sprintf(output, "1 - In the IF, time is %u\r\n", temporary_time);
	UART_puts(UART_GNSS, output);
	//-------------------
	TimerEnable(TIMER1_BASE, TIMER_A);					//start timer1
	LED_toggle(LED_B);
	}
	//for test only---------
	else
	{
	sprintf(output, "Watchdog not kicked, difference to big, wdt time is %u\r\n", temporary_time);
	UART_puts(UART_GNSS,output);
	}
	//---------------------
}
//Timer1 interrupt handler
void Timer1IntHandler(void)
{
	UART_puts(UART_GNSS, "In second - lowering - interrupt\r\n");
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
	ulPeriod = (SysCtlClockGet() / 10) * 19;						//divide the period corresponding to 1s by 10, to have value of timer corresponding to 0.1s; *19 to have a period for 1.9s
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
//function to update the timestamp in the mission loop;
//watchdog interrupt handler is comparing how much the timestamp from the mission loop
//differ from the time at the interrupt moment; if the difference is bigger than 120sec
//software is found to be hang up and the watchdog kick will not occur leading to RESET
void update_watchdog_timestamp(){
      RTC_getTime(&missionloop_time);
}