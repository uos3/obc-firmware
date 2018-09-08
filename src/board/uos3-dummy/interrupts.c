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
#include "../interrupts.h"

// this code is evolved from an online purdue uni tutorial and the ti peripheral driver manual

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "../led.h"

#define MAX_TIMER 65535

#define DEBUG_B 2
#define DEBUG_A 1
#define DEBUG_C 3

uint16_t timer = 0;
uint32_t task_times_1[] = {5,8}; // 32-bit to prevent overflow during edge addition
uint32_t task_times_2[] = {2,6};
uint32_t curr_task_times_1[] = {0,0};
uint32_t curr_task_times_2[] = {0,0};

uint8_t current_led = DEBUG_A;

// void event_EnableIRQ (void){
//   __asm (
//     "MRS     r1, CPSR"
//     "BIC     r1, r1, #0x80"
//     "MSR     CPSR_c, r1"
//   );
// }

void ExampleMode1_ISR(void)
{
  GPIO_set(DEBUG_C);
  Delay_ms(100);
  GPIO_reset(DEBUG_C);

  // Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  //TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // enable interrupt
  //TimerEnable(TIMER0_BASE, TIMER_A); 	 // start timer

  if (current_led == DEBUG_A){
    current_led = DEBUG_B;
    IntPendSet(TIMER1_BASE);
  } else {
    current_led = DEBUG_A;
  }
}

void LEDA_ISR(void){
  GPIO_set(DEBUG_A);
  Delay_ms(1500);
  GPIO_reset(DEBUG_A);
}

void LEDB_ISR(void){
  GPIO_set(DEBUG_B);
  Delay_ms(1500);
  GPIO_reset(DEBUG_B);
}

void setup_interrupts(void)
 {
 	unsigned long ulPeriod;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // turn on timer
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {} // wait for timer to be ready
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // set it to periodically trigger interrupt
	ulPeriod = (SysCtlClockGet()) ; // 1Hz update rate
	 // third will miss sometimes if processor busy
	TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod-1); // prime it
	TimerIntRegister(TIMER0_BASE, TIMER_A, ExampleMode1_ISR); // this is for dynamic interrupt compilation
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // enable interrupt
	TimerEnable(TIMER0_BASE, TIMER_A); 	 // start timer

  // Enable interrupts on timer 2 and 3
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // turn on timer
  TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
  TimerIntRegister(TIMER1_BASE, TIMER_A, LEDA_ISR);
  TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2); // turn on timer
  TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
  TimerIntRegister(TIMER2_BASE, TIMER_A, LEDB_ISR);
  TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
 }
