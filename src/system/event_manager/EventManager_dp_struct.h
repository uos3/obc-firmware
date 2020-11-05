/**
 * @ingroup event_manager
 * 
 * @file EventManager_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides DataPool parameters for the EventManager
 * @version 0.1
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

/* Internal includes */
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
     * @brief Stores an error code coming from the EventManager.
     * 
     * @dp 2
     */
    EventManager_ErrorCode ERROR_CODE;

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
    uint8_t NUM_RAISED_EVENTS;

    /**
     * @brief The current size of the allocated event lists.
     * 
     * This value must be capable of storing the value of
     * EVENTMANAGER_MAX_LIST_SIZE, which currently is 256. Therefore a 16 bit
     * value is used here.
     * 
     * @dp 5
     */
    uint16_t EVENT_LIST_SIZE;


} EventManager_Dp;

#endif /* H_EVENTMANAGER_DP_STRUCT_H */