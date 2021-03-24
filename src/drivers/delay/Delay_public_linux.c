/**
 * @file Delay_public_linux.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Public implementation of the Delay module for the linux target.
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
#include <time.h>

/* Internal includes */
#include "drivers/delay/Delay_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void Delay_ms(uint32_t milliseconds_in) {
    Delay_us(milliseconds_in * DELAY_MICROS_TO_MILLIS);
}

void Delay_us(uint32_t microseconds_in) {
    clock_t start;
    clock_t end;

    /* Get start and end time of the delay */
    start = clock();
    end = start + (microseconds_in * (CLOCKS_PER_SEC / 1000000));
    
    /* Wait for the delay to end */
    while (clock() < end) {}
}