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
/**
 * @variable
 * @ingroup watchdog_ext
 * 
 * Stores the integer value responsible for the watchdog reset,
 * variable  is set in the mission loop to the value 0xFF,
 * and decremented by 1 in each watchdog interrupt handler,
 * if the mission loop will stuck, the variable will not be set back to 0XFF
 * and in result will be 0 or smaller after some time, when this happened, watchdog is not kicked and MCU will reboot
 */
volatile uint8_t watchdog_update;
/**
 * @function
 * @ingroup watchdog_ext
 * 
 * Enables the external watchdog kick with the timers responsible for that operation
 */
void enable_watchdog_kick(void);
#endif