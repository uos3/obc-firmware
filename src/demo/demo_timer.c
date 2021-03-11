/**
 * @file demo_timer.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Demonstrates usage of the Timer functionality
 * @version 0.1
 * @date 2021-02-03
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/timer/Timer_public.h"

/* Only included for test, don't use in real code */
#include "drivers/timer/Timer_private.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
    ErrorCode error = ERROR_NONE;
    bool run_loop = true;
    Event timer_0_25_done;
    Event timer_2_done;
    Event timer_1_done;
    Event timer_10_done;
    bool is_raised = false;
    int num_0_25_timers = 0;
    int num_1_timers = 0;
    int num_2_timers = 0;
    bool timer_disabled = false;
    
    /* Init system critical modules */
    Kernel_init_critical_modules();


    DEBUG_INF("Timer Demo");

    /* Init the timer module */
    error = Timer_init();
    if (error != ERROR_NONE) {
        Debug_exit(1);
    }

    /* Start 0.25s timer (periodic) */
    DEBUG_INF("Starting 0.25 second periodic (repeating) timer");
    error = Timer_start_periodic(0.25, &timer_0_25_done);
    if (error != ERROR_NONE) {
        Debug_exit(1);
    }
    DEBUG_INF("Timer event code: 0x%04X", timer_0_25_done);

    DEBUG_INF("");

    /* Start 1s timer (periodic) */
    DEBUG_INF("Starting 1 second periodic (repeating) timer");
    error = Timer_start_periodic(1.0, &timer_1_done);
    if (error != ERROR_NONE) {
        Debug_exit(1);
    }
    DEBUG_INF("Timer event code: 0x%04X", timer_1_done);

    DEBUG_INF("");

    /* Start 10s timer */
    DEBUG_INF("Starting 10 second one-shot timer");
    error = Timer_start_one_shot(10.0, &timer_10_done);
    if (error != ERROR_NONE) {
        Debug_exit(1);
    }
    DEBUG_INF("Timer event code: 0x%04X", timer_10_done);

    DEBUG_INF("");

    /* Start 2s timer */
    DEBUG_INF("Starting 2 second periodic timer");
    error = Timer_start_periodic(2.0, &timer_2_done);
    if (error != ERROR_NONE) {
        Debug_exit(1);
    }
    DEBUG_INF("Timer event code: 0x%04X", timer_2_done);

    /* Run main loop */
    while (run_loop) {
        /* Increment number of 0.25 s events */
        if (!EventManager_poll_event(timer_0_25_done, &is_raised)) {
            Debug_exit(1);
        }
        if (is_raised) {
            num_0_25_timers++;
        }

        /* Count number of 1 second timers */
        if (!EventManager_poll_event(timer_1_done, &is_raised)) {
            Debug_exit(1);
        }
        if (is_raised) {
            num_1_timers++;
        }
        if (num_1_timers >= 5 && !timer_disabled) {
            /* Disable the 1 s timer after 5 events counted */
            error = Timer_disable(timer_1_done);
            if (error != ERROR_NONE) {
                Debug_exit(1);
            }

            timer_disabled = true;
        }

        /* Count number of 2 s timer events */
        if (!EventManager_poll_event(timer_2_done, &is_raised)) {
            Debug_exit(1);
        }
        if (is_raised) {
            num_2_timers++;
        }

        /* Check for end at 10 s timer */
        if (!EventManager_poll_event(timer_10_done, &is_raised)) {
            Debug_exit(1);
        }
        if (is_raised) {
            run_loop = false;
        }

        if (!EventManager_cleanup_events()) {
            Debug_exit(1);
        }
    }

    DEBUG_INF(
        "10 s timer fired, %dx1 s timers, %dx2 s timers, and %dx0.25 s timers", 
        num_1_timers,
        num_2_timers,
        num_0_25_timers
    );

    DEBUG_INF("Should only be 5x1 s timers, as after 5 we disable that timer.");
    DEBUG_INF("Should have 4x2 s timers, since they are started after the 10 s timer.");
    DEBUG_INF("The 0.25 s timers are started before the 10 s, so there should be 40 of them.");

    /* Disable all timers then destroy the event manager. Must be done in this
     * order to prevent a hardfault if a timer attempts to access the event
     * manager after it's destroyed. */
    Timer_disable_all();
    EventManager_destroy();

    return 0;
}