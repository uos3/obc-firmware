/**
* @defgroup watchdog_ext
*
* @brief External Watchdog Driver
*
* @details Driver for controlling and kicking external watchdog
*
* @ingroup drivers
*/

// this is development code to aid in implementing watchdog kicking on interrupt

// not necessarily in Uos3 Cubesat flight software - risk that automatically kicking on interrupt

// might prevent watchdog timer resetting when operation has actually failed

// could set a flag regularly, but actual kicking should be done as far as possible explicitly

//

// Two ways to initialise the interrupt. Dynamically with TimerIntRegister, or statically by setting
// the define variable TIMER0A to the routine name (with my modified tm4c_startup_gcc.c this works)

// (C) Suzanna Lucarotti 20/9/17

// Developed for Uos3 Cubesat

#ifndef _WDT_EXT_H_
#define _WDT_EXT_H_

#include "../firmware.h"
//by md9u17
volatile uint8_t watchdog_update;
void Timer0IntHandler(void);                //interrupt routine for setting high wdt kick signal and start Timer1 to count 10us
void Timer1IntHandler(void);                //interrupt routine for setting low wdt kick signal after required hold time 10us
void setinterrupt_wdt_risingedge(void);     //setting Timer0 for the interrupt and period 1.9s
void setinterrupt_wdt_fallingedge(void);    //setting Timer1 for the interrupt and period 10us; Timer1 is started by Timer0IntHandler
void enable_watchdog_kick(void);            //function to set the Timers 0 and 1 and therefore set watchdog kick
void update_watchdog_timestamp(void);       //for getting current time in mission loop
#endif