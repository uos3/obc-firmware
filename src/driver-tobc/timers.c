#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#include "../driver/timers.h"
#include "../driver/delay.h"

#define TIMERS_AMOUNT_OF_TIMERS 6

// small static header section, don't want to include in main header
void timer_handler_from_number(uint32_t timer_number);

void timer0_handler_wrapper();
void timer1_handler_wrapper();
void timer2_handler_wrapper();
void timer3_handler_wrapper();
void timer4_handler_wrapper();
void timer5_handler_wrapper();


// TM4C hardware registers for timers
static uint32_t timers_registers[] = {
	WTIMER0_BASE,
	WTIMER1_BASE,
	WTIMER2_BASE,
	WTIMER3_BASE,
	WTIMER4_BASE,
	WTIMER5_BASE
};

// TM4C system peripheral registers, for enabling the modules
static uint32_t sysperipheral_registers[] = {
	SYSCTL_PERIPH_WTIMER0,     // Wide Timer 0
	SYSCTL_PERIPH_WTIMER1,     // Wide Timer 1
	SYSCTL_PERIPH_WTIMER2,     // Wide Timer 2
	SYSCTL_PERIPH_WTIMER3,     // Wide Timer 3
	SYSCTL_PERIPH_WTIMER4,     // Wide Timer 4
	SYSCTL_PERIPH_WTIMER5
};

// timer interrupt handlers ==================================================


void timer0_handler_wrapper(){
	timer_handler_from_number(0);
}

void timer1_handler_wrapper(){
	timer_handler_from_number(1);
}

void timer2_handler_wrapper(){
	timer_handler_from_number(2);
}

void timer3_handler_wrapper(){
	timer_handler_from_number(3);
}

void timer4_handler_wrapper(){
	timer_handler_from_number(4);
}

void timer5_handler_wrapper(){
	timer_handler_from_number(5);
}

void (*handler_wrappers[TIMERS_AMOUNT_OF_TIMERS])(void) = {
	timer0_handler_wrapper,
	timer1_handler_wrapper,
	timer2_handler_wrapper,
	timer3_handler_wrapper,
	timer4_handler_wrapper,
	timer5_handler_wrapper
};

static void (*handler_fns[TIMERS_AMOUNT_OF_TIMERS])(void);


void timer_handler_from_number(uint32_t timer_number){
	TimerIntClear(timers_registers[timer_number], TIMER_BOTH);
	handler_fns[timer_number]();
}

// enables the timers; required due to being a peripheral
static void enable_timer_peripheral(uint32_t timer_number){
	SysCtlPeripheralEnable(sysperipheral_registers[timer_number]);
	while(!SysCtlPeripheralReady(sysperipheral_registers[timer_number])){
		Delay_ms(10);
	}
}

static uint64_t system_seconds_conversion;

/*
	inits the timer modules and configures for periodic use. does not start
	individual timers
*/ 
void timers_init(){
	// frequency in Hz is the same as the number of cycles per second by definition
	// to convers seconds to ticks, just use this
	system_seconds_conversion = SysCtlClockGet();
	for (int timer = 0; timer < TIMERS_AMOUNT_OF_TIMERS; timer++){
		// enable the peripherals before using (duh)
		enable_timer_peripheral(timer);
		// configure the timers for periodic mode
		TimerConfigure(timers_registers[timer], TIMER_CFG_PERIODIC);
	}
}

uint8_t timers_setup_timer(uint32_t timer_num, uint64_t period_s, void (*handler_fnptr)(void)){
	if (timer_num >= TIMERS_AMOUNT_OF_TIMERS){
		// prevents random assignment to non-existant timers in public functions
		return 1;
	}
	// === the following functions are all from driverlib/timer.h ===
	// sets the timeout period for the timer in seconds.
	TimerLoadSet64(timers_registers[timer_num], system_seconds_conversion*period_s);
	// assigns the function pointer to it's place in the array, rather than
	// directly to the interrupt. This allows for a before and after processes
	// to be added to the handler  (eg TimerIntClear()) WITHOUT having to
	// directly include it the function that performs the task desired when
	// the timer expires
	handler_fns[timer_num] = handler_fnptr;
	// passes handler_wrappers, as these contain TimerIntClear for clearing the
	// interrupt once it fires, preventing softlock
	TimerIntRegister(timers_registers[timer_num], TIMER_BOTH, handler_wrappers[timer_num]);
	// enables the interrupt, which will fire when the timer expires
	TimerIntEnable(timers_registers[timer_num], TIMER_BOTH);
	// enables the timer to start counting down.
	TimerEnable(timers_registers[timer_num], TIMER_BOTH);
	return 0;
}

void timers_stop_all(){
	for (int timer = 0; timer < TIMERS_AMOUNT_OF_TIMERS; timer++){
		// hazard a wild guess at what happens here.
		TimerIntDisable(timers_registers[timer], TIMER_BOTH);
		TimerDisable(timers_registers[timer], TIMER_BOTH);
		// tasks must be reassigned to be restarted.
	}
}
