// this is development code to aid in implementing watchdog kicking on interrupt

// not necessarily in Uos3 Cubesat flight software - risk that automatically kicking on interrupt

// might prevent watchdog timer resetting when operation has actually failed

// could set a flag regularly, but actual kicking should be done as far as possible explicitly

//

// Two ways to initialise the interrupt. Dynamically with TimerIntRegister, or statically by setting
// the define variable TIMER0A to the routine name (with my modified tm4c_startup_gcc.c this works)

// (C) Suzanna Lucarotti 20/9/17

// Developed for Uos3 Cubesat


//#define TIMER0A Timer0IntHandler // this is for static interrupt compilation, needs to be before building
// tm4c_startup_gcc.c

#include "../firmware.h"

// this code is evolved from an online purdue uni tutorial and the ti peripheral driver manual

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

void setupwatchdoginterrupt(void); // this routine enables regular watchdog kicking on interrupt
