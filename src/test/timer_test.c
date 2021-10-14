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

#define ACTIVITY_TIMEOUT_MS (10000)

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {

    ErrorCode error = ERROR_NONE;
    bool run_loop = true;
    /* Amount of one shot timers that will be initialised */
    int one_shot_timers_amount = 10;
    Event timer_0_25_done;
    bool timer_disabled = false;
    int num_of_0_25_fired_timers = 0;

    /* Init system critical modules */
    Kernel_init_critical_modules();

    DEBUG_INF(" ---- TIMER TEST ----");

    /* Init the timer module and check for errors within the timer module start-up */
    error = Timer_init();
    if (error != ERROR_NONE)
    {
        Debug_exit(1);
    }

    /* Start a 0.25s one shot timer (For testing) */
    DEBUG_INF("Activating a 0.25s one shot timer");
    error = Timer_start_one_shot(0.25, &timer_0_25_done);
    if (error = ERROR_NONE) 
    {
        Debug_exit(1);
    }

    DEBUG_INF("Timer event code: 0x%04X", timer_0_25_done);

    /* Run main loop */
    while (run_loop)
    {
        if (EventManager_poll_event(timer_0_25_done))
        {
            /* Keep track how many 0.25s one shot timer were fired */
            num_of_0_25_fired_timers++;
        }

        if (num_of_0_25_fired_timers >= one_shot_timers_amount) {
            /* Only start the desired amount of one shot timers specified */
            
        }

        
        
    }
    
    
    



    
    return 0;
}


