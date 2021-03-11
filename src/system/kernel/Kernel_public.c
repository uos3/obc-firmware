/**
 * @file Kernel_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Public definition for the Kernel.
 * 
 * Task ref: [UT_2.9.12]
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* External includes */
#ifdef TARGET_TM4C
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#endif

/* System includes */
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/* FIXME: Implement critical section stacking?? */

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

void Kernel_init_critical_modules(void) {

    /*
     * A note on rebooting:
     * If any of these modules fail to init something is seriously wrong, and
     * we likely have lost the mission. There's a very limit number of things
     * we can do here, but we can try to reboot the system.
     * TODO:
     *  - Should we try to clean 
     */

    DataPool_init();
    Board_init();

    /* Debug can fail if it can't get a valid initial time on linux. */
    if (!Debug_init()) {
        Kernel_reboot();
    }

    /* EventManager can fail if there's not enough memory for the event arrays.
     * This would be a pretty critical problem, but there's _probably_ noting
     * we can do about it in flight. As this is such a critical system it's
     * failure would be unrecoverable.
     * 
     * TODO: potentially we shouldn't try to reboot because we'd end up drawing
     * too much power? */
    if (!EventManager_init()) {
        Kernel_reboot();
    }

    /* TODO: Init Fdir */
}

void Kernel_reboot(void) {
    /* This wouldn't work if debug hasn't been init, but if that's the case we
     * just miss this print message, it won't cause additional failures */
    DEBUG_ERR("==== KERNEL REBOOT STARTED ====");

    /* TODO: Write monitoring data (num reboots etc.) to EEPROM */

    /*
     * Trigger the reboot of the system
     *
     * If on Linux we won't actually reboot, instead just exit. However if
     * we're on the TM4C we need to us the tivaware driver to trigger a
     * software reset. Therefore this is one of the only non-driver modules to
     * link to tivaware.
     */
    #ifdef TARGET_UNIX
    exit(EXIT_FAILURE);
    #elif TARGET_TM4C
    SysCtlReset();
    #endif

}