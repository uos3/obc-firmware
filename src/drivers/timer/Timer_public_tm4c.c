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
#include "system/kernel/Kernel_public.h"
#include "drivers/timer/Timer_public.h"
#include "drivers/timer/Timer_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Timer_init(void) {
    uint8_t block;
    uint8_t checks;
    uint8_t num_failed_peripherals = 0;
    bool ready;

    /* Set the timer module as enabled at the start */
    TIMER_MODULE_DISABLED = false;

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

        /* If the peripheral isn't ready increment the counter for the number
         * of failed peripherals. If only some of them fail we can still do a
         * lot of the work of this module! */
        if (!ready) {
            DEBUG_WRN("Could not enable Timer block %d", block);
            /* TODO: generate failure TM */
            num_failed_peripherals++;
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

    /* Check if the number of failed peripherals matches the number of timers
     * we have, if so none of them are working and we must error out and 
     * disable the timer driver. */
    if (num_failed_peripherals >= TIMER_NUM_BLOCKS) {
        DEBUG_ERR(
            "All timer peripherals failed to initialise, disabling timer module"
        );
        TIMER_MODULE_DISABLED = true;
        return TIMER_ERROR_MODULE_DISABLED;
    }

    /* Timer configuration is performed when a timer is started */

    return ERROR_NONE;
}

ErrorCode Timer_start_one_shot(
    double duration_s_in, 
    Event *p_timer_event_out
) {
    /* Check for disabled module */
    if (TIMER_MODULE_DISABLED) {
        return TIMER_ERROR_MODULE_DISABLED;
    }

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
    /* Check for disabled module */
    if (TIMER_MODULE_DISABLED) {
        return TIMER_ERROR_MODULE_DISABLED;
    }

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

ErrorCode Timer_disable(Event timer_event_in) {
    int timer_idx, i;
    bool timer_found = false;
    Timer_Timer *p_timer;

    /* Check for disabled module */
    if (TIMER_MODULE_DISABLED) {
        return TIMER_ERROR_MODULE_DISABLED;
    }

    /* Check the event is one of the timer events */
    if (timer_event_in < EVT_TIMER_00A_COMPLETE 
        || 
        timer_event_in > EVT_TIMER_11B_COMPLETE
    ) {
        DEBUG_ERR(
            "Cannot disable timer as event 0x%04X isn't associated with a timer",
            timer_event_in
        );
        return TIMER_ERROR_NON_TIMER_EVENT;
    }

    /* Get the timer index associated with this event. As events are linear
     * increments from EVT_TIMER_00A_COMPLETE we can just work out the 
     * difference and use that as an index. */
    timer_idx = timer_event_in - EVT_TIMER_00A_COMPLETE;

    /* Check that the timer is actually the correct one by accessing that timer
     * and making sure the event IDs match. If they don't match do a linear
     * search through the timer array for the right index. */
    if (TIMER_STATE.timers[timer_idx].completed_event != timer_event_in) {
        for (i = 0; i < TIMER_NUM_TIMERS; ++i) {
            if (TIMER_STATE.timers[i].completed_event == timer_event_in) {
                timer_idx = i;
                timer_found = true;
                break;
            }
        }

        /* If the timer wasn't found return an error */
        if (!timer_found) {
            DEBUG_ERR(
                "Couldn't find timer associated with event 0x%04X", 
                timer_event_in
            );
            return TIMER_ERROR_TIMER_EVENT_NOT_FOUND;
        }
    }

    /* Get a pointer to the timer for easier manipulation */
    p_timer = &TIMER_STATE.timers[timer_idx];

    /* If the timer isn't disabled don't error but show a warning. */
    if (p_timer->is_available) {
        DEBUG_WRN("Cannot disable timer as it isn't enabled");
        return ERROR_NONE;
    }

    Timer_disable_specific(p_timer);

    return ERROR_NONE;
}

void Timer_disable_all(void) {
    int timer_idx;

    /* Check for disabled module */
    if (TIMER_MODULE_DISABLED) {
        return TIMER_ERROR_MODULE_DISABLED;
    }
    
    /* Loop over all timers and disable them if they are enabled */
    for (timer_idx = 0; timer_idx < TIMER_NUM_TIMERS; ++timer_idx) {
        if (!TIMER_STATE.timers[timer_idx].is_available) {
            Timer_disable_specific(&TIMER_STATE.timers[timer_idx]);
        }
    }
}