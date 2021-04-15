/**
 * @ingroup event_manager
 * 
 * @file EventManager_private.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides private functions for the EventManager
 * 
 * @version 2.0
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdlib.h>
#include <stdint.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/event_manager/EventManager_private.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

EventManager EVENTMANAGER;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void EventManager_shrink_lists(void) {
    /* Note: previously a check of DP.EVENTMANAGER.INITIALISED was made here,
     * and an error return if it wasn't init. This has been removed to make
     * this function infallible. Since EventManager is now counted as a
     * critical module it shall be initialised before any other module could
     * use this function */

    Event *p_temp_raised_events;
    uint8_t *p_temp_num_cycles_raised;

    /* Check if the lists need to be shrunk */
    if (
        DP.EVENTMANAGER.NUM_RAISED_EVENTS * EVENTMANAGER_SHRINK_LIST_MULTIPLIER
        <=
        DP.EVENTMANAGER.EVENT_LIST_SIZE
    ) {
        /* Get new list size */
        size_t new_list_size = 
            DP.EVENTMANAGER.EVENT_LIST_SIZE 
            / EVENTMANAGER_SHRUNK_LIST_SIZE_DIVISOR;

        /* If the new size is less than the min size clamp to the min size.
         * By preventing the lists getting too small the number of reallocs
         * required is reduced. */
        if (new_list_size < EVENTMANAGER_MIN_LIST_SIZE) {
            new_list_size = EVENTMANAGER_MIN_LIST_SIZE;
        }

        /* If a reallocation is required.
         * This is done since the new list size could be clamped, and want to
         * avoid reallocating to the same size. */
        if (new_list_size != DP.EVENTMANAGER.EVENT_LIST_SIZE) {
            /* Realloc the lists. We use temporary pointers so that if the
             * realloc fails the original pointers will be left untouched.
             * Failing to shrink the lists won't prevent further execution of
             * the EM code, so we will simply inform FDIR of the failed
             * attempt and leave the lists as they are. */
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

                /* Set the shrink realloc error and raise the error event */
                DP.EVENTMANAGER.ERROR.code
                    = EVENTMANAGER_ERROR_SHRINK_REALLOC_FAILED;
                
                /* TODO: inform FDIR of error here */
            }
            /* If it succeeded, move the pointers and update the new size of
             * the lists */
            else {
                DEBUG_TRC(
                    "EventManager list size increased to %lu", 
                    new_list_size
                );
                EVENTMANAGER.p_raised_events = p_temp_raised_events;
                EVENTMANAGER.p_num_cycles_events_raised 
                    = p_temp_num_cycles_raised;
                DP.EVENTMANAGER.EVENT_LIST_SIZE = new_list_size;
            }

        }
    }
}

void EventManager_remove_stale_events(void) {
    /* Allocate an array to store events that have gone stale (not been polled
     * for 2 cycles ) and need to be removed */
    Event *p_stale_events = (Event *)malloc(
        DP.EVENTMANAGER.NUM_RAISED_EVENTS * sizeof(Event)
    );
    uint8_t stale_len = 0;

    /* Loop through events */
    for (uint8_t i = 0; i < DP.EVENTMANAGER.NUM_RAISED_EVENTS; i++) {
        /* If the event has been raised for more than two cycles add it to the
         * stale events array */
        if (EVENTMANAGER.p_num_cycles_events_raised[i] 
            >= 
            EVENTMANAGER_STALE_EVENT_CYCLE_THRESHOLD
        ) {
            p_stale_events[stale_len] = EVENTMANAGER.p_raised_events[i];
            stale_len++;
        }
    }

    /* If there are stale events poll them */
    if (stale_len > 0) {
        for (uint8_t i = 0; i < stale_len; i++) {
            EventManager_poll_event(p_stale_events[i]);
            DEBUG_TRC(
                "Event 0x%04X was polled as part of cleanup", 
                p_stale_events[i]
            );
        }
    }

    /* Free the stale events array */
    free(p_stale_events);
}
