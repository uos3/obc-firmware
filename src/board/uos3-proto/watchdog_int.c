// this is development code to aid in implementing watchdog kicking on interrupt

// not necessarily in Uos3 Cubesat flight software - risk that automatically kicking on interrupt

// might prevent watchdog timer resetting when operation has actually failed

// could set a flag regularly, but actual kicking should be done as far as possible explicitly

//

// Two ways to initialise the interrupt. Dynamically with TimerIntRegister, or statically by setting
// the define variable TIMER0A to the routine name (with my modified tm4c_startup_gcc.c this works)

// (C) Suzanna Lucarotti 20/9/17

// Developed for Uos3 Cubesat


//#define TIMER0A Timer0IntHandler // this is for static interrupt compilation

//#include "../firmware.h"
#include "board.h"
#include "../watchdog_int.h"

// this code is evolved from an online purdue uni tutorial and the ti peripheral driver manual

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
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // turn on timer
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {} // wait for timer to be ready
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // set it to periodically trigger interrupt
	ulPeriod = (SysCtlClockGet()  / 4) ; // how often it triggers (counts down) (half will miss watchdog)
	 // third will miss sometimes if processor busy
	TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod-1); // prime it
	TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0IntHandler); // this is for dynamic interrupt compilation
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // enable interrupt
	TimerEnable(TIMER0_BASE, TIMER_A); 	 // start timer
 }
