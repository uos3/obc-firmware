/**
 * @file timer_test.c
 * @author Gianluca Borgo (gb2g20@soton.ac.uk/gianborgo@hotmail.com)
 * @brief This will test the functionality of the timer
 * 
 * 
 * 
 * @version 0.1
 * @date 2021-10-22
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/timer/Timer_public.h"

/* Defines when the 0.20s one shot timers will be fired */
Event fire_one_shot_timers(Event *timer_one_shot_event, int amount) {

    /* Start a 0.20s one shot timer (For testing) */
    // TODO: Check if the for loop is causing it run out of timers, then use a function that will only activate a one short timer when the other timer has completed.
    DEBUG_INF("Activating several 0.20s one shot timers");
    for (int i = 0; i < amount; i++) {
        /* Checking for errors when the timer is initialized */
        bool error = Timer_start_one_shot(0.20, &timer_one_shot_event[i]);
        if (error = ERROR_NONE) {
            DEBUG_INF("Could not initialize 0.20s one shot timer");
            Debug_exit(1);
        }

        DEBUG_INF("Timer event code: 0x%04X", timer_one_shot_event[i]);
    }
}

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {

    ErrorCode error = ERROR_NONE;
    bool run_loop = true;
    /* Amount of one shot timers that will be initialised */
    int one_shot_timers_amount = 24;
    /* This will be an array of events fired every 0.20s by the one shot timer */
    Event timer_0_20_done[one_shot_timers_amount];
    /* This will be the event fired every second to test periodic timers */
    Event timer_1_periodic_done;
    /* Starting one shot timer to verify simple functionality of the system */
    Event timer_1_one_shot_done;
    bool timer_disabled = false;
    int num_of_0_20_fired_timers = 0;
    int num_of_1_timers = 0;
    /* This will be used to measure how long should the 1s periodic timer should run for */
    int periodic_run_time_required = 10;

    /* Init system critical modules */
    Kernel_init_critical_modules();

    if (!Debug_init()) {
        return 0;
    }

    DEBUG_INF(" ---- TIMER TEST ----");
    
    /* Required initialisations and verifications for the Datapool, EventManager, and Board */
    DEBUG_INF("Init DP...");
    DataPool_init();
    if (!DP.INITIALISED) {
        DEBUG_INF("Could not initialize the datapool, exiting...");
        Debug_exit(1);
    }

    DEBUG_INF("Init EventManager...");
    if (!EventManager_init()) {
        DEBUG_INF("Could not initialize EventManager");
        Debug_exit(1);
    }

    DEBUG_INF("Init Board...");
    Board_init();
    if (!DP.BOARD_INITIALISED) {
        DEBUG_INF("Could not initialize the Board");
        Debug_exit(1);
    }

    /* Init the timer module and check for errors within the timer module start-up */
    error = Timer_init();
    if (error != ERROR_NONE) {
        DEBUG_INF("Could not initialize the Timer");
        Debug_exit(1);
    }

    DEBUG_INF("Activating a 1s one shot timer to verify simple functionality");
    error = Timer_start_one_shot(1, &timer_1_one_shot_done);
    if (error != ERROR_NONE) {
        DEBUG_INF("Could not initialize the 1s one shot timer");
        Debug_exit(1);
    }

    DEBUG_INF("Starting a 1s periodic timer");
    error = Timer_start_periodic(1, &timer_1_periodic_done);
    if (error != ERROR_NONE) {
        DEBUG_INF("Could not initialize the 1s periodic timer");
        Debug_exit(1);
    }

    /* Run main loop */
    while (run_loop) {
        if (EventManager_poll_event(timer_1_periodic_done)) {
            /* Keep track how many 1s timers were fired */
            num_of_1_timers++;
        }

        if (num_of_1_timers >= periodic_run_time_required && !timer_disabled) {
            /* Disable all timers to enable the one shot timers to be enabled */
            fire_one_shot_timers(timer_0_20_done, one_shot_timers_amount);
            timer_disabled = true;
        }

        if (EventManager_poll_event(timer_0_20_done)) {
            num_of_0_20_fired_timers++;
        }

        if (num_of_0_20_fired_timers >= one_shot_timers_amount) {
            run_loop = false;
        }

        EventManager_cleanup_events();
    }
    
    /* Disable all timers then destroy the event manager. Must be done in this
     * order to prevent a hardfault if a timer attempts to access the event
     * manager after it's destroyed. */
    Timer_disable_all();
    EventManager_destroy();
    
    return 0;
}
