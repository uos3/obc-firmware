/**
 * @ingroup event_manager
 * 
 * @file EventManager_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides EventManager module functionality.
 * 
 * See corresponding header file for more information.
 * 
 * @version 1.0
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

/* Internal includes */
#include "util/debug/Debug_public.h"
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
        /* Free the lists */
        free(EVENTMANAGER.p_raised_events);
        free(EVENTMANAGER.p_num_cycles_events_raised);

        /* Zero the manager */
        memset(&EVENTMANAGER, 0, sizeof(EVENTMANAGER));
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
    /* Check that init has been called */
    if (DP.EVENTMANAGER.INITIALISED == false) {
        /* Debug log */
        DEBUG_ERR("Attempted to raise event when EVENTMANAGER not initialised");
        
        /* Raise erorr code, don't raise event as the EM isn't init */
        DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_NOT_INITIALISED;
        return false;
    }

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
        size_t new_list_size = DP.EVENTMANAGER.EVENT_LIST_SIZE * 2;

        /* If the new size is greater than the max size raise an error */
        if (new_list_size > EVENTMANAGER_MAX_LIST_SIZE) {
            /* Debug log */
            DEBUG_ERR("Maximum number of events reached");

            /* Raise the error code and flag */
            DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_MAX_EVENTS_REACHED;
            DP.EVENTMANAGER.MAX_EVENTS_REACHED = true;
            return false;
        }

        /* Reallocate the lists */
        EVENTMANAGER.p_raised_events = (Event *)realloc(
            (void *)EVENTMANAGER.p_raised_events, 
            new_list_size * sizeof(Event)
        );
        EVENTMANAGER.p_num_cycles_events_raised = (uint8_t *)realloc(
            (void *)EVENTMANAGER.p_num_cycles_events_raised, 
            new_list_size * sizeof(uint8_t)
        );

        /* Check reallocation succeeded */
        if (
            EVENTMANAGER.p_raised_events == NULL
            ||
            EVENTMANAGER.p_num_cycles_events_raised == NULL
        ) {
            /* Debug log */
            DEBUG_ERR("Error reallocating memory for EVENTMANAGER");

            /* Raise error code */
            DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_OUT_OF_MEMORY;
            return false;
        }

        /* Update the size of the list */
        DP.EVENTMANAGER.EVENT_LIST_SIZE = new_list_size;
    }

    /* Raise a new event */
    EVENTMANAGER.p_raised_events[DP.EVENTMANAGER.NUM_RAISED_EVENTS] = event_in;

    /* Set number of cycles for this event to zero */
    EVENTMANAGER.p_num_cycles_events_raised[DP.EVENTMANAGER.NUM_RAISED_EVENTS] 
        = 0;

    /* Increment number of events */
    DP.EVENTMANAGER.NUM_RAISED_EVENTS++;

    /* Return success */
    return true;
}

bool EventManager_is_event_raised(Event event_in, bool *p_is_raised_out) {
    /* If not initialised error */
    if (!DP.EVENTMANAGER.INITIALISED) {
        /* Debug log */
        DEBUG_ERR("Attempted to raise event when EVENTMANAGER not initialised");
        
        /* Raise erorr code, don't raise event as the EM isn't init */
        DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_NOT_INITIALISED;
        return false;
    }

    /* Set the default flag to false for checking if an event is raised */
    *p_is_raised_out = false;
    
    /* Check to see if the event is in the array of currently raised events */
    int i;
    for (i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS; ++i) {
        if (EVENTMANAGER.p_raised_events[i] == event_in) {
            /* If the event to check is in the array of currently raised
             * events, set p_is_raised_out to true */
            *p_is_raised_out = true;
            break;
        }
    }

    return true;
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
    if (!EventManager_shrink_lists()) {
        return false;
    }

    return true;
}

bool EventManager_poll_event(Event event_in, bool *p_is_raised_out) {
    /* If not initialised error */
    if (!DP.EVENTMANAGER.INITIALISED) {
        /* Debug log */
        DEBUG_ERR("Attempted to raise event when EVENTMANAGER not initialised");
        
        /* Raise erorr code, don't raise event as the EM isn't init */
        DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_NOT_INITIALISED;
        return false;
    }

    /* Get whether or not the event is raised */
    if (!EventManager_is_event_raised(event_in, p_is_raised_out)) {
        return false;
    }

    /* If raised, remove the event from the event list */
    if (*p_is_raised_out) {
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
         * the counters for the number of cycles each event has been raised.
         */
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
        if (!EventManager_shrink_lists()) {
            /* Error occured */
            return false;
        }
    }

    return true;
}

bool EventManager_cleanup_events(void) {
    /* If not initialised error */
    if (!DP.EVENTMANAGER.INITIALISED) {
        /* Debug log */
        DEBUG_ERR("Attempted to raise event when EVENTMANAGER not initialised");
        
        /* Raise erorr code, don't raise event as the EM isn't init */
        DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_NOT_INITIALISED;
        return false;
    }

    /* Allocate an array to store events that have gone stale (not been polled
     * for 2 cycles ) and need to be removed */
    Event *p_stale_events = (Event *)malloc(
        DP.EVENTMANAGER.NUM_RAISED_EVENTS * sizeof(Event)
    );
    uint8_t stale_len = 0;

    /* Loop through events */
    for (uint8_t i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS; i++) {
        /* Increment the number of cycles for this event, as this function is
         * to be called at the end of each cycle. */
        EVENTMANAGER.p_num_cycles_events_raised[i]++;

        /* If the event has been raised for more than two cycles add it to the
         * stale events array */
        if (EVENTMANAGER.p_num_cycles_events_raised[i] >= 2) {
            p_stale_events[stale_len] = EVENTMANAGER.p_raised_events[i];
            stale_len++;
        }
    }

    /* If there are stale events poll them */
    if (stale_len > 0) {
        bool is_raised = false;

        for (uint8_t i = 0; i < stale_len; i++) {
            if (!EventManager_poll_event(p_stale_events[i], &is_raised)) {
                return false;
            }
        }
    }

    /* Free the stale events array */
    free(p_stale_events);

    return true;
}