/**
 * @ingroup event_manager
 * 
 * @file EventManager_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides DataPool parameters for the EventManager
 * @version 2.0
 * @date 2020-10-30
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_EVENTMANAGER_DP_STRUCT_H
#define H_EVENTMANAGER_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdbool.h>
#include <stdlib.h>

/* Internal includes */
#include "system/kernel/Kernel_errors.h"
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

typedef struct _EventManager_Dp {

    /**
     * @brief Flag indicating whether or not the EventManager has been 
     * initialised.
     * 
     * @dp 1
     */
    bool INITIALISED;

    /**
     * @brief Stores errors that can occur in the EventManager.
     * 
     * @dp 2
     */
    Error ERROR;

    /**
     * @brief Flag which is true if the maximum number of events have been
     * raised, indicating that some events may be missed.
     * 
     * @dp 3
     */
    bool MAX_EVENTS_REACHED;

    /**
     * @brief Counter storing the number of raised events.
     * 
     * @dp 4
     */
    uint16_t NUM_RAISED_EVENTS;

    /**
     * @brief The current size of the allocated event lists.
     * 
     * This value must be capable of storing the value of
     * EVENTMANAGER_MAX_LIST_SIZE, which currently is 256. Therefore size_t,
     * which is 32 bits on the TM4C, is sufficient.
     * 
     * @dp 5
     */
    size_t EVENT_LIST_SIZE;


} EventManager_Dp;

#endif /* H_EVENTMANAGER_DP_STRUCT_H */