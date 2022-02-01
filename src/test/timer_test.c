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

/* -------------------------------------------------------------------------   
 * CONSTANTS
 * ------------------------------------------------------------------------- */

#define TIMERS_AMOUNT 24

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {

    ErrorCode error = ERROR_NONE;
    bool run_loop = true;
    /* This will be an array of events fired every 0.20s by the one shot timer */
    Event timer_0_20_done;
    //Event timer_0_20_done;
    /* This will be the event fired every second to test periodic timers */
    Event timer_1_periodic_done;
    /* Starting one shot timer to verify simple functionality of the system */
    Event timer_1_one_shot_done;
    bool timer_disabled = false;
    int num_of_0_20_fired_timers = 0;
    int num_of_1_one_shot_timers = 0;
    int num_of_1_timers = 0;
    /* This will be used to measure how long should the 1s periodic timer should run for */
    int periodic_run_time_required = 20;

    /* Init system critical modules */
    Kernel_init_critical_modules();

    if (!Debug_init()) {
        return 0;
    }

    DEBUG_INF("---------------------");
    DEBUG_INF(" ---- TIMER TEST ----");
    DEBUG_INF("---------------------");
    
    /* Required initialisations and verifications for the Datapool, EventManager, and Board */
    DEBUG_INF("Init DP...");
    DataPool_init();
    if (!DP.INITIALISED) {
        DEBUG_INF("Could not initialize the datapool, exiting...");
        Debug_exit(1);
    }

    DEBUG_INF("");

    DEBUG_INF("Init EventManager...");
    if (!EventManager_init()) {
        DEBUG_INF("Could not initialize EventManager");
        Debug_exit(1);
    }

    DEBUG_INF("");

    DEBUG_INF("Init Board...");
    Board_init();
    if (!DP.BOARD_INITIALISED) {
        DEBUG_INF("Could not initialize the Board");
        Debug_exit(1);
    }

    DEBUG_INF("");

    /* Init the timer module and check for errors within the timer module start-up */
    error = Timer_init();
    if (error != ERROR_NONE) {
        DEBUG_INF("Could not initialize the Timer");
        Debug_exit(1);
    }

    DEBUG_INF("");

    DEBUG_INF("Activating a 1s one shot timer to verify simple functionality");
    error = Timer_start_one_shot(1, &timer_1_one_shot_done);
    if (error != ERROR_NONE) {
        DEBUG_INF("Could not initialize the 1s one shot timer");
        Debug_exit(1);
    }

    DEBUG_INF("");

    DEBUG_INF("Starting a 1s periodic timer");
    error = Timer_start_periodic(1, &timer_1_periodic_done);
    if (error != ERROR_NONE) {
        DEBUG_INF("Could not initialize the 1s periodic timer");
        Debug_exit(1);
    }

    DEBUG_INF("");

    /* Run main loop */
    while (run_loop) {

        // Check if timer of 0.2s has polled.
        if (EventManager_poll_event(timer_0_20_done)) {
            DEBUG_INF("Polled 0.2s one shot timer");
            num_of_0_20_fired_timers++;
        }

        if (EventManager_poll_event(timer_1_one_shot_done)) {
            num_of_1_one_shot_timers++;
            DEBUG_INF("Simple functionality test for a 1s one shot timer");
        }
        

        if (EventManager_poll_event(timer_1_periodic_done)) {
            /* Initializing a 0.2s one shot timer every second*/
            /* Checking for errors when the timer is initialized */
            bool error = Timer_start_one_shot(0.20, &timer_0_20_done);
            if (error = ERROR_NONE) {
                DEBUG_INF("Could not initialize 0.20s one shot timer");
                Debug_exit(1);
            }
            DEBUG_INF("Timer event code: 0x%04X", timer_0_20_done);
            /* Keep track how many 1s timers were fired */
            num_of_1_timers++;
        }

        /* TODO: Make it so that the periodic timer is running for the amount of time required to launch a single one shot timer everytime the periodic timer is triggered.
         * It will run for a specific amount of time until either the num_of_0_20_fired_timers >= TIMERS_AMOUNT or until it has reached a maximum run time */
        if (num_of_1_timers >= periodic_run_time_required && !timer_disabled) {

            /* Disable the 1s periodic timer after it ran its required time */
            error = Timer_disable(timer_1_periodic_done);
            if (error != ERROR_NONE) {
                DEBUG_INF("Error disabling the timer exiting...");
                Debug_exit(1);
            }
            timer_disabled = true;
            run_loop = false;
        }

        if (num_of_0_20_fired_timers >= TIMERS_AMOUNT) {
            run_loop = false;
        }

        EventManager_cleanup_events();
    }
    DEBUG_INF("The test ran for a total of %ds. In %ds had a total of %dx0.2s timers and %dx1s one shot timer.", periodic_run_time_required, num_of_1_timers, num_of_0_20_fired_timers, num_of_1_one_shot_timers);
    
    /* Disable all timers then destroy the event manager. Must be done in this
     * order to prevent a hardfault if a timer attempts to access the event
     * manager after it's destroyed. */
    Timer_disable_all();
    EventManager_destroy();
    
    return 0;
}
