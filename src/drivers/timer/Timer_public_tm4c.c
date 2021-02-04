/**
 * @file Timer_public_tm4c.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Implements the timer driver for the software on the TM4C platform.
 * 
 * Task ref: [UT_2.8.1]
 * 
 * @version 0.1
 * @date 2021-02-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* System includes */
#include <stdint.h>
#include <stdbool.h>

/* Tivaware includes */
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/timer/Timer_public.h"
#include "drivers/timer/Timer_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Timer_init(void) {
    uint8_t block;
    uint8_t checks;
    bool ready;

    /* Enable all timer blocks */
    /* TODO: Dynamically enable/disable timers when not in use? */
    for (block = 0; block < TIMER_NUM_BLOCKS; ++block) {
        /* Enable the timer */
        SysCtlPeripheralEnable(TIMER_STATE.blocks[block].peripheral);

        ready = false;

        /* Loop a limited number of times checking if the peripheral is ready
         * yet */
        for (checks = 0; 
            checks < TIMER_MAX_NUM_PERIPH_READY_CHECKS; 
            ++checks
        ) {
            if (SysCtlPeripheralReady(TIMER_STATE.blocks[block].peripheral)) {
                ready = true;
                break;
            }
        }

        /* If the peripheral isn't ready issue error */
        if (!ready) {
            /* TODO: Mark the module as disabled? */
            DEBUG_ERR("Could not enable Timer block %d", block);
            return TIMER_ERROR_PERIPH_ENABLE_FAILED;
        }

        /* Register the interrupts for the timers */
        TimerIntRegister(
            TIMER_STATE.blocks[block].base, 
            TIMER_A, 
            TIMER_STATE.blocks[block].p_a->p_interrupt_handler
        );
        TimerIntRegister(
            TIMER_STATE.blocks[block].base, 
            TIMER_B, 
            TIMER_STATE.blocks[block].p_b->p_interrupt_handler
        );
        
    }

    /* Timer configuration is performed when a timer is started */

    return ERROR_NONE;
}

ErrorCode Timer_start_one_shot(
    double duration_s_in, 
    Event *p_timer_event_out
) {
    ErrorCode error = Timer_configure_timer_for_duration(
        duration_s_in,
        false,
        p_timer_event_out
    );

    if (error != ERROR_NONE) {
        DEBUG_ERR("Error while configuring timer: 0x%04d", error);
    }
    
    return error;
}

ErrorCode Timer_start_periodic(
    double duration_s_in, 
    Event *p_timer_event_out
) {
    ErrorCode error = Timer_configure_timer_for_duration(
        duration_s_in,
        true,
        p_timer_event_out
    );

    if (error != ERROR_NONE) {
        DEBUG_ERR("Error while configuring timer: 0x%04d", error);
    }
    
    return error;
}