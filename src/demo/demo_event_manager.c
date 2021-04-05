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
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
/* It is bad practice to include a module's private header but since this is a
 * demo (and partial test) we need access to the EVENTMANAGER global. */
#include "system/event_manager/EventManager_private.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {

    /* Init system */
    DataPool_init();
    Board_init();
    if (!Debug_init()) {
        Debug_exit(1);
    }

    DEBUG_INF("EventManager demo");

    /* Init event manager */
    if (!EventManager_init()) {
        Debug_exit(1);   
    }

    /* Raise an event */
    for (int event = 1; event < 32; ++event) {
        if (!EventManager_raise_event((Event)event)) {
            Debug_exit(1);   
        }
    }

    /* Print events */
    DEBUG_INF("Events:");
    printf("   [");
    for (int i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS; ++i) {
        printf("%d ", EVENTMANAGER.p_raised_events[i]);
    }
    printf("]\n");

    /* Print size of lists */
    DEBUG_INF("List size: %ld", DP.EVENTMANAGER.EVENT_LIST_SIZE);


    /* Poll many off the list to verify that the lists will shrink */
    DEBUG_INF("Polling 30 events");
    for (int event = 1; event < 30; event++) {
        EventManager_poll_event((Event)event);
    }

    /* Print events */
    DEBUG_INF("Events:");
    printf("   [");
    for (int i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS; ++i) {
        printf("%d ", EVENTMANAGER.p_raised_events[i]);
    }
    printf("]\n");

    /* Print size of lists */
    DEBUG_INF("List size: %ld\n", DP.EVENTMANAGER.EVENT_LIST_SIZE);

    if (!EventManager_raise_event(256)) {
        Debug_exit(1);   
    }

    /* Clean up events three times to check that the stale events are polled */
    for (int i = 0; i < 3; i++) {
        DEBUG_INF("Cleaning events");
        EventManager_cleanup_events();

        /* Print events */
        DEBUG_INF("Events:");
        printf("   [");
        for (int i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS; ++i) {
            printf("%d ", EVENTMANAGER.p_raised_events[i]);
        }
        printf("]\n");

        /* Print size of lists */
        DEBUG_INF("List size: %ld", DP.EVENTMANAGER.EVENT_LIST_SIZE);
    }

    /* Destroy event manager */
    EventManager_destroy();
}

/** @} */ /* End of demo_event_manager */