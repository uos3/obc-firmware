/**
 * @ingroup demo
 * 
 * @file demo_event_manager.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Short demo of using the EventManager
 * @version 0.1
 * @date 2020-10-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup demo_event_manager Demo Event Manager
 * @{
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* Internal includes */
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main() {

    printf("EventManager demo\n");

    /* Init event manager */
    if (!EventManager_init()) {
        exit(1);   
    }

    /* Raise an event */
    for (int event = 1; event < 32; ++event) {
        if (!EventManager_raise_event(event)) {
            exit(1);   
        }
    }

    /* Print events */
    printf("Events: [");
    for (int i = 0; i < EVENTMANAGER.num_raised_events; ++i) {
        printf("%d ", EVENTMANAGER.p_raised_events[i]);
    }
    printf("]\n");

    /* Print size of lists */
    printf("List size: %ld\n", EVENTMANAGER.size_of_lists);


    /* Poll many off the list to verify that the lists will shrink */
    printf("Polling 30 events\n");
    bool is_raised = false;
    for (int event = 1; event < 30; event++) {
        if (!EventManager_poll_event(event, &is_raised)) {
            exit(1);   
        }
    }

    /* Print events */
    printf("Events: [");
    for (int i = 0; i < EVENTMANAGER.num_raised_events; ++i) {
        printf("%d ", EVENTMANAGER.p_raised_events[i]);
    }
    printf("]\n");

    /* Print size of lists */
    printf("List size: %ld\n", EVENTMANAGER.size_of_lists);

    if (!EventManager_raise_event(256)) {
        exit(1);   
    }

    /* Clean up events three times to check that the stale events are polled */
    for (int i = 0; i < 3; i++) {
        printf("Cleaning events\n");
        if (!EventManager_cleanup_events()) {
            exit(1);
        }

        /* Print events */
        printf("Events: [");
        for (int i = 0; i < EVENTMANAGER.num_raised_events; ++i) {
            printf("%d ", EVENTMANAGER.p_raised_events[i]);
        }
        printf("]\n");

        /* Print size of lists */
        printf("List size: %ld\n", EVENTMANAGER.size_of_lists);
    }

    /* Destroy event manager */
    EventManager_destroy();
}