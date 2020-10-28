/**
 * @ingroup event_manager
 * 
 * @file EventManager_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides EventManager module functionality.
 * 
 * See corresponding header file for more information.
 * 
 * @version 0.1
 * @date 2020-10-28
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Internal includes */
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief The global EventManager instance
 */
EventManager EVENTMANAGER;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief If the current number of raised events is less than 1/4 the allocated 
 * list size the lists shall be shrunk by 1/2.
 * 
 * @return true The lists were successfully shrunk.
 * @return false An error occured.
 */
bool EventManager_shrink_lists() {
    return true;
}

bool EventManager_init() {
    /* Zero the manager */
    memset(&EVENTMANAGER, 0, sizeof(EVENTMANAGER));

    /* Allocate the lists to their minimum size
     * 
     * ---- NUMERICAL PROTECTION ----
     * The multiplications below could result in an integer overflow. However
     * the definition of EVENTMANAGER_MIN_LIST_SIZE shall ensure that the array
     * is less than the overflow size of 32 bits.
     */
    EVENTMANAGER.p_raised_events = (Event *)malloc(
        EVENTMANAGER_MIN_LIST_SIZE * sizeof(Event)
    );
    EVENTMANAGER.p_num_cycles_events_raised = (Event *)malloc(
        EVENTMANAGER_MIN_LIST_SIZE * sizeof(Event)
    );

    /* Check allocation was successful */
    if (
        EVENTMANAGER.p_raised_events == NULL
        ||
        EVENTMANAGER.p_num_cycles_events_raised == NULL
    ) {
        // TODO: log error
        printf("Error allocating memory for EVENTMANAGER\n");
        return false;
    }

    /* Set the size and counter members */
    EVENTMANAGER.size_of_lists = EVENTMANAGER_MIN_LIST_SIZE;
    EVENTMANAGER.num_raised_events = 0;

    /* Set the initialised member */
    EVENTMANAGER.initialised = true;

    /* Return success */
    return true;
}

void EventManager_destroy() {
    /* Free the lists */
    free(EVENTMANAGER.p_raised_events);
    free(EVENTMANAGER.p_num_cycles_events_raised);

    /* Zero the manager */
    memset(&EVENTMANAGER, 0, sizeof(EVENTMANAGER));
}

bool EventManager_raise_event(Event event_in) {
    /* Check that init has been called */
    if (EVENTMANAGER.initialised == false) {
        // TODO: log error
        printf("Attempted to raise event when EVENTMANAGER not initialised\n");
        return false;
    }

    /* Increase list size if the number of raised events has reached the
     * current size of the list. */
    if (EVENTMANAGER.num_raised_events >= EVENTMANAGER.size_of_lists - 1) {
        /* Calculate the new size of the list */
        size_t new_list_size = EVENTMANAGER.size_of_lists * 2;

        /* If the new size is greater than the max size raise an error */
        if (new_list_size > EVENTMANAGER_MAX_LIST_SIZE) {
            // TODO: log error
            printf("Maximum number of events reached\n");
            return false;
        }

        /* Reallocate the lists */
        EVENTMANAGER.p_raised_events = (void *)realloc(
            EVENTMANAGER.p_raised_events, 
            new_list_size * sizeof(Event)
        );
        EVENTMANAGER.p_num_cycles_events_raised = (void *)realloc(
            EVENTMANAGER.p_num_cycles_events_raised, 
            new_list_size * sizeof(Event)
        );

        /* Check reallocation succeeded */
        if (
            EVENTMANAGER.p_raised_events == NULL
            ||
            EVENTMANAGER.p_num_cycles_events_raised == NULL
        ) {
            // TODO: log error
            printf("Error allocating memory for EVENTMANAGER\n");
            return false;
        }

        /* Update the size of the list */
        EVENTMANAGER.size_of_lists = new_list_size;

        // TODO: REMOVE
        printf("Event list size increased\n");
    }

    /* Raise a new event */
    EVENTMANAGER.p_raised_events[EVENTMANAGER.num_raised_events] = event_in;

    /* Set number of cycles for this event to zero */
    EVENTMANAGER.p_num_cycles_events_raised[EVENTMANAGER.num_raised_events] 
        = 0;

    /* Increment number of events */
    EVENTMANAGER.num_raised_events += 1;

    /* Return success */
    return true;
}