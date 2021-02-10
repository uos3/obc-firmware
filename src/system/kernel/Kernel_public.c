/**
 * @file Kernel_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Public definition for the Kernel.
 * 
 * TODO:
 * Task ref: []
 * 
 * @version 0.1
 * @date 2021-02-10
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * IMPORTS
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

/* External includes */
#ifdef TARGET_TM4C
#include "driverlib/interrupt.h"
#endif

/* System includes */
#include "system/kernel/Kernel_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void Kernel_disable_interrupts(void) {
    #ifdef TARGET_TM4C
    IntMasterDisable();
    #endif
}

void Kernel_enable_interrupts(void) {
    #ifdef TARGET_TM4C
    IntMasterEnable();
    #endif
}