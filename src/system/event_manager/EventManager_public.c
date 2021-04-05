/**
 * @ingroup event_manager
 * 
 * @file EventManager_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides EventManager module functionality.
 * 
 * See corresponding header file for more information.
 * 
 * @version 2.0
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
#include <stdint.h>
#include <string.h>
#ifdef DEBUG_MODE
#include <stdio.h>
#endif

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/event_manager/EventManager_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool EventManager_init(void) {
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
    EVENTMANAGER.p_num_cycles_events_raised = (uint8_t *)malloc(
        EVENTMANAGER_MIN_LIST_SIZE * sizeof(uint8_t)
    );

    /* Check allocation was successful */
    if (
        EVENTMANAGER.p_raised_events == NULL
        ||
        EVENTMANAGER.p_num_cycles_events_raised == NULL
    ) {
        /* Debug log */
        DEBUG_ERR("Error allocating memory for EVENTMANAGER");

        /* Since the event manager itself isn't allocated we just set the error
         * code in the data pool and return false, rather than raising the
         * eventmanager error event. */
        DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_OUT_OF_MEMORY;
        return false;
    }

    /* Set the size and counter members */
    DP.EVENTMANAGER.EVENT_LIST_SIZE = EVENTMANAGER_MIN_LIST_SIZE;
    DP.EVENTMANAGER.NUM_RAISED_EVENTS = 0;

    /* Set the initialised member */
    DP.EVENTMANAGER.INITIALISED = true;

    /* Return success */
    return true;
}

void EventManager_destroy(void) {
    /* If the event manager is intiailised destroy it */
    if (DP.EVENTMANAGER.INITIALISED) {
        /* Zero the manager's datapool. We do this before freeing the lists
         * since an event could be raised by an interrupt, meaning that we
         * could concievably free the list before setting the manager as
         * uninitialised, which will cause a hard fault when the interrupt
         * tries to raise an event using a free'd list (use after free). */
        memset(&DP.EVENTMANAGER, 0, sizeof(DP.EVENTMANAGER));

        /* Free the lists if not NULL */
        if (EVENTMANAGER.p_raised_events != NULL) {
            free(EVENTMANAGER.p_raised_events);
        }
        if (EVENTMANAGER.p_num_cycles_events_raised != NULL) {
            free(EVENTMANAGER.p_num_cycles_events_raised);
        }
    }
    /* Otherwise warn that destroy was called twice */
    else {
        DEBUG_WRN(
            "EventManager_destroy() was called when the EventManager wasn't \
            initialised."
        );
    }

}

bool EventManager_raise_event(Event event_in) {
    Event *p_temp_raised_events;
    uint8_t *p_temp_num_cycles_raised;

    /* Raise event must be interrupt safe, as it could be called in an
     * interrupt, therefore we disable interrupts for the duration of this 
     * function
     * 
     * TODO: IS THIS NECESSARY??? */
    Kernel_disable_interrupts();

    DEBUG_TRC("EVENT: 0x%04X", event_in);

    /* Note: previously a check of DP.EVENTMANAGER.INITIALISED was made here,
     * and an error return if it wasn't init. This has been removed since 
     * EventManager is counted as a critical module it shall be initialised 
     * before any other module could use this function */

    /* Increase list size if the number of raised events has reached the
     * current size of the list. */
    if (DP.EVENTMANAGER.NUM_RAISED_EVENTS 
        >= 
        DP.EVENTMANAGER.EVENT_LIST_SIZE - 1
    ) {

        /* Calculate the new size of the list 
         *
         * ---- NUMERICAL PROTECTION ----
         * This operation could cause EVENT_LIST_SIZE to overflow. Therefore
         * the maximum list size must be set lower than the maximum value
         * representable by EVENT_LIST_SIZE data type.
         */
        size_t new_list_size 
            = DP.EVENTMANAGER.EVENT_LIST_SIZE 
            * EVENTMANAGER_GROW_LIST_MULTIPLIER;

        /* If the new size is greater than the max size raise an error */
        if (new_list_size > EVENTMANAGER_MAX_LIST_SIZE) {
            /* Debug log */
            DEBUG_ERR("Maximum number of events reached");

            /* Raise the error code and flag */
            DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_MAX_EVENTS_REACHED;
            DP.EVENTMANAGER.MAX_EVENTS_REACHED = true;
            return false;
        }

        /* Reallocate the lists. We use temporary values here so that if the
         * realloc fails the original lists remain in place. A failure to
         * reallocate here would actually prevent a new event being raised. The
         * likelyhood of reallocation failing (i.e. running out of heap size)
         * is very small, so instead of erroring out here we will instead raise
         * an error for FDIR. */
        p_temp_raised_events = (Event *)realloc(
            (void *)EVENTMANAGER.p_raised_events, 
            new_list_size * sizeof(Event)
        );
        p_temp_num_cycles_raised = (uint8_t *)realloc(
            (void *)EVENTMANAGER.p_num_cycles_events_raised, 
            new_list_size * sizeof(uint8_t)
        );

        /* Check reallocation failed */
        if (
            p_temp_raised_events == NULL
            ||
            p_temp_num_cycles_raised == NULL
        ) {
            /* Debug log */
            DEBUG_ERR("Error reallocating memory for EVENTMANAGER");

            /* Raise error code */
            DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_OUT_OF_MEMORY;
            return false;
        }
        /* Or if it succeeded, set the new pointers and list size */
        else {
            DEBUG_TRC("EventManager list size increased to %lu", new_list_size);
            EVENTMANAGER.p_raised_events = p_temp_raised_events;
            EVENTMANAGER.p_num_cycles_events_raised = p_temp_num_cycles_raised;
            DP.EVENTMANAGER.EVENT_LIST_SIZE = new_list_size;
        }
    }

    /* Raise a new event */
    EVENTMANAGER.p_raised_events[DP.EVENTMANAGER.NUM_RAISED_EVENTS] = event_in;

    /* Set number of cycles for this event to zero */
    EVENTMANAGER.p_num_cycles_events_raised[DP.EVENTMANAGER.NUM_RAISED_EVENTS] 
        = 0;

    /* Increment number of events */
    DP.EVENTMANAGER.NUM_RAISED_EVENTS++;

    /* Reenable interrupts */
    Kernel_enable_interrupts();

    /* Return success */
    return true;
}

bool EventManager_is_event_raised(Event event_in) {
    /* Note: previously a check of DP.EVENTMANAGER.INITIALISED was made here,
     * and an error return if it wasn't init. This has been removed to make
     * this function infallible. Since EventManager is now counted as a
     * critical module it shall be initialised before any other module could
     * use this function */

    /* Set the default flag to false for checking if an event is raised */
    bool is_raised = false;
    
    /* Check to see if the event is in the array of currently raised events */
    int i;
    for (i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS; ++i) {
        if (EVENTMANAGER.p_raised_events[i] == event_in) {
            /* If the event to check is in the array of currently raised
             * events, set the flag to true and exit here, no need to check
             * other events */
            is_raised = true;
            break;
        }
    }

    return is_raised;
}

bool EventManager_clear_all_events(void) {
    /* If not initialised error */
    if (!DP.EVENTMANAGER.INITIALISED) {
        /* Debug log */
        DEBUG_ERR("Attempted to raise event when EVENTMANAGER not initialised");
        
        /* Raise erorr code, don't raise event as the EM isn't init */
        DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_NOT_INITIALISED;
        return false;
    }

    memset(
        EVENTMANAGER.p_raised_events, 
        0, 
        DP.EVENTMANAGER.NUM_RAISED_EVENTS * sizeof(Event)
    );
    
    /* Shrink the lists */
    EventManager_shrink_lists();

    return true;
}

bool EventManager_poll_event(Event event_in) {
    /* Note: previously a check of DP.EVENTMANAGER.INITIALISED was made here,
     * and an error return if it wasn't init. This has been removed to make
     * this function infallible. Since EventManager is now counted as a
     * critical module it shall be initialised before any other module could
     * use this function */

    /* If the event isn't raised exit here */
    if (!EventManager_is_event_raised(event_in)) {
        return false;
    }

    /* If the event was raised remove it from the list */

    /* First have to find the position of the event in the list.
     * 
     * ---- NUMERICAL PROTECTION ----
     * The event must be in the list since it is raised, so there is no
     * need to check for the event not being in the list.
     */
    uint8_t event_idx = 0;
    for (uint8_t i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS; ++i) {
        if (EVENTMANAGER.p_raised_events[i] == event_in) {
            event_idx = i;
            break;
        }
    }

    /* Iterate through the list and move all events after the polled event
     * to the left. This has to be done for both the events themsevles and
     * the counters for the number of cycles each event has been raised. */
    for (
        uint8_t i = event_idx;
        i < DP.EVENTMANAGER.NUM_RAISED_EVENTS;
        ++i
    ) {
        /* Set the current value to the next value */
        EVENTMANAGER.p_raised_events[i] 
            = EVENTMANAGER.p_raised_events[i + 1];
        EVENTMANAGER.p_num_cycles_events_raised[i]
            = EVENTMANAGER.p_num_cycles_events_raised[i + 1];
    }

    /* Set the final elements to 0 */
    EVENTMANAGER.p_raised_events[DP.EVENTMANAGER.NUM_RAISED_EVENTS] = 0;
    EVENTMANAGER.p_num_cycles_events_raised[
        DP.EVENTMANAGER.NUM_RAISED_EVENTS
    ] = 0;

    /* Reduce the number of raised events by 1 */
    DP.EVENTMANAGER.NUM_RAISED_EVENTS--;

    /* Reduce the size of the lists if required by the new size */
    EventManager_shrink_lists();

    return true;
}

void EventManager_cleanup_events(void) {
    /* Note: previously a check of DP.EVENTMANAGER.INITIALISED was made here,
     * and an error return if it wasn't init. This has been removed to make
     * this function infallible. Since EventManager is now counted as a
     * critical module it shall be initialised before any other module could
     * use this function */

    /* Loop through events */
    for (uint8_t i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS; i++) {
        /* Increment the number of cycles for this event, as this function is
         * to be called at the end of each cycle. */
        EVENTMANAGER.p_num_cycles_events_raised[i]++;
    }

    /* Clean stale events */
    EventManager_remove_stale_events();
}

#ifdef DEBUG_MODE
void EventManager_get_event_list_string(char **pp_str_out) {
    /* Events will be listed like [0x0000, 0x0001, 0x0002], so the length to
     * allocate for the string is 8*NUM_RAISED_EVENTS + 1 for the null byte */
    *pp_str_out = (char *)malloc(
        sizeof(char) * ((8 * (size_t)DP.EVENTMANAGER.NUM_RAISED_EVENTS) + 1)
    );
    /* Char buf for easy concat of the event IDs */
    char buf[9] = {0};

    /* Print the opening bracket */
    sprintf(*pp_str_out, "[");

    /* Loop through all events except the last one */
    for (int i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS - 1; ++i) {
        /* Format the event ID and concat with the string */
        sprintf((char *)buf, "0x%04X, ", EVENTMANAGER.p_raised_events[i]);
        strcat(*pp_str_out, (char *)buf);
    }

    /* Print the last one with the end bracket too */
    sprintf(
        (char *)buf, 
        "0x%04X]", 
        EVENTMANAGER.p_raised_events[DP.EVENTMANAGER.NUM_RAISED_EVENTS - 1]
    );

    /* Concat the last one */
    strcat(*pp_str_out, (char *)buf);
}
#endif