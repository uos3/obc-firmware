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
#include <stdio.h> // TODO: remove once logging added
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
    /* Check if the lists need to be shrunk */
    if (
        EVENTMANAGER.num_raised_events * EVENTMANAGER_SHRINK_LIST_MULTIPLIER
        <=
        EVENTMANAGER.size_of_lists
    ) {
        /* Get new list size */
        uint8_t new_list_size = 
            EVENTMANAGER.size_of_lists / EVENTMANAGER_SHRUNK_LIST_SIZE_DIVISOR;

        /* If the new size is less than the min size clamp to the min size.
         * By preventing the lists getting too small the number of reallocs
         * required is reduced. */
        if (new_list_size < EVENTMANAGER_MIN_LIST_SIZE) {
            new_list_size = EVENTMANAGER_MIN_LIST_SIZE;
        }

        /* If a reallocation is required.
         * This is done since the new list size could be clamped, and want to
         * avoid reallocating to the same size. */
        if (new_list_size != EVENTMANAGER.size_of_lists) {
            /* Realloc the lists */
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
                // TODO: log error
                printf("Error reallocating memory for EVENTMANAGER\n");
                return false;
            }

            /* Update the size */
            EVENTMANAGER.size_of_lists = new_list_size;
        }
    }

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
    EVENTMANAGER.p_num_cycles_events_raised = (uint8_t *)malloc(
        EVENTMANAGER_MIN_LIST_SIZE * sizeof(uint8_t)
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
            // TODO: log error
            printf("Error reallocating memory for EVENTMANAGER\n");
            return false;
        }

        /* Update the size of the list */
        EVENTMANAGER.size_of_lists = new_list_size;
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

bool EventManager_poll_event(Event event_in, bool *p_is_raised_out) {
    /* By default set the is_raised to false, to prevent attempting to remove a
     * non-raised event later on. */
    *p_is_raised_out = false;

    /* First use is_raised to check if the event was raised. */
    *p_is_raised_out = true;
    // TODO: temp while this func is being written
    // if (!EventManager_is_event_raised(event_in, p_is_raised_out)) {
    //     /* An error occured, return false */
    //     return false;
    // }

    /* If raised, remove the event from the event list */
    if (*p_is_raised_out) {
        /* First have to find the position of the event in the list.
         * 
         * ---- NUMERICAL PROTECTION ----
         * The event must be in the list since it is raised, so there is no
         * need to check for the event not being in the list.
         */
        uint8_t event_idx = 0;
        for (uint8_t i = 0; i < EVENTMANAGER.num_raised_events; ++i) {
            if (EVENTMANAGER.p_raised_events[i] == event_in) {
                event_idx = i;
                break;
            }
        }

        /* Iterate through the list and move all events after the polled event
         * to the left. This has to be done for both the events themsevles and
         * the counters for the number of cycles each event has been raised.
         */
        for (uint8_t i = event_idx; i < EVENTMANAGER.num_raised_events; ++i) {
            /* Set the current value to the next value */
            EVENTMANAGER.p_raised_events[i] 
                = EVENTMANAGER.p_raised_events[i + 1];
            EVENTMANAGER.p_num_cycles_events_raised[i]
                = EVENTMANAGER.p_num_cycles_events_raised[i + 1];
        }

        /* Set the final elements to 0 */
        EVENTMANAGER.p_raised_events[EVENTMANAGER.num_raised_events] = 0;
        EVENTMANAGER.p_num_cycles_events_raised[EVENTMANAGER.num_raised_events]
             = 0;

        /* Reduce the number of raised events by 1 */
        EVENTMANAGER.num_raised_events -= 1;

        /* Reduce the size of the lists if required by the new size */
        if (!EventManager_shrink_lists()) {
            /* Error occured */
            return false;
        }
    }

    return true;
}

bool EventManager_cleanup_events() {

    /* Allocate an array to store events that have gone stale (not been polled
     * for 2 cycles ) and need to be removed */
    Event *p_stale_events = (Event *)malloc(
        EVENTMANAGER.num_raised_events * sizeof(Event)
    );
    uint8_t stale_len = 0;

    /* Loop through events */
    for (uint8_t i = 0; i < EVENTMANAGER.num_raised_events; i++) {
        /* Increment the number of cycles for this event, as this function is
         * to be called at the end of each cycle. */
        EVENTMANAGER.p_num_cycles_events_raised[i] += 1;

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