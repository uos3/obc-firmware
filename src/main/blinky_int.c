/* firmware.h contains all relevant headers */

/* A very simple example that blinks the on-board LED. */

// now modified to watchdog kick on interrupt

#include "../firmware.h"

int main(void)
{
    Board_init();
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // turn on clock

    WDT_kick();
    LED_off(LED_B);
    Delay_ms(1000); // so can be clearly seen when watchdog kicking

    setupwatchdoginterrupt();	// kick the watchdog on interrupt

	while(1)
    {
     LED_on(LED_B);Delay_ms(100);
     LED_off(LED_B);Delay_ms(100);
    }

}

/*
int main(void)
{
  Board_init();
  WDT_kick();

  LED_off(LED_B);

    Delay_ms(1000);


  while(1)
  {
    LED_on(LED_B);

 
    Delay_ms(500);
  //  WDT_kick();

    LED_off(LED_B);

    
    Delay_ms(200);
//    WDT_kick();
  }
}
*/
/*
//#define TIMER0A Timer0IntHandler // this is for static interrupt compilation

// this code based on example from online purdue uni tutorial

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

void Timer0IntHandler(void) 
{
// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    WDT_kick(); // kick the watchdog
}
void setupwatchdoginterrupt(void)
 {
	unsigned long ulPeriod;
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // turn on clock
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // turn on timer
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {} // wait for timer to be ready
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // set it to periodically trigger interrupt
	ulPeriod = (SysCtlClockGet() / 10) / 2; // how often it triggers (counts down)
	TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod-1); // prime it
	TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0IntHandler); // this is for dynamic interrupt compilation
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // enable interrupt
	TimerEnable(TIMER0_BASE, TIMER_A); 	 // start timer
 }
*/




/*
void Timer0IntHandler(void) //this is the version that toggles LED on timed interrupt
{
// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
// Read the current state of the GPIO pin and
// write back the opposite state
	if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1)) // this is LED B
	{
	//	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		LED_off(LED_B);
	}
	else
	{
	//	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
		LED_on(LED_B);
	}
}
*/