/**
 * @ingroup event_manager
 * 
 * @file EventManager_private.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides private functions for the EventManager
 * 
 * @version 0.1
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
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool EventManager_shrink_lists(void) {
    /* Check that init has been called */
    if (!DP.EVENTMANAGER.INITIALISED) {
        /* Debug log */
        DEBUG_ERR("Attempted to raise event when EVENTMANAGER not initialised");
        
        /* Raise erorr code, don't raise event as the EM isn't init */
        DP.EVENTMANAGER.ERROR_CODE = EVENTMANAGER_ERROR_NOT_INITIALISED;
        return false;
    }

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
                /* Debug log */
                DEBUG_ERR("Error reallocating memory for EVENTMANAGER");

                /* Set the shrink realloc error and raise the error event */
                DP.EVENTMANAGER.ERROR_CODE 
                    = EVENTMANAGER_ERROR_SHRINK_REALLOC_FAILED;
                EventManager_raise_event(EVT_EVENTMANAGER_ERROR);
                return false;
            }

            /* Update the size */
            DP.EVENTMANAGER.EVENT_LIST_SIZE = new_list_size;
        }
    }

    return true;
}
