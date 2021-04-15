/**
 * @file Delay_public_tm4c.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Public implementation of the Delay module for the TM4C target.
 * 
 * Task ref: [UT_2.8.3]
 * 
 * @version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

/* External includes */
#include "driverlib/sysctl.h"

/* Internal includes */
#include "drivers/delay/Delay_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void Delay_ms(uint32_t milliseconds_in) {
    Delay_us(milliseconds_in * DELAY_MICROS_TO_MILLIS);
}

void Delay_us(uint32_t microseconds_in) {
    SysCtlDelay(microseconds_in * DELAY_CYCLES_PER_MICRO);
}