/**
 * @ingroup event_manager
 * @file EventManager_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Defines error codes for the EventManager
 * 
 * Task ref: [UT_2.9.1]
 * 
 * @version 0.1
 * @date 2020-11-30
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_EVENTMANAGER_ERRORS_H
#define H_EVENTMANAGER_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include "system/kernel/Kernel_module_ids.h"
#include "system/kernel/Kernel_errors.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The maximum number of events has been reached and no more events
 * will be raised until an event is polled.
 */
#define EVENTMANAGER_ERROR_MAX_EVENTS_REACHED ((ErrorCode)MOD_ID_EVENTMANAGER | 1)

/**
 * @brief There was insufficient memory to increase the size of the event 
 * lists.
 */
#define EVENTMANAGER_ERROR_OUT_OF_MEMORY ((ErrorCode)MOD_ID_EVENTMANAGER | 2)

/**
 * @brief A call to realloc() failed while trying to shrink the lists. This
 * indicates memory corruption. 
 */
#define EVENTMANAGER_ERROR_SHRINK_REALLOC_FAILED ((ErrorCode)MOD_ID_EVENTMANAGER | 3)

/**
 * @brief An EventManager function was called when the EventManager wasn't
 * initialised. Make sure to call EventManager_init() before using the
 * EventManager.
 */
#define EVENTMANAGER_ERROR_NOT_INITIALISED  ((ErrorCode)MOD_ID_EVENTMANAGER | 4)

#endif /* H_EVENTMANAGER_ERRORS_H */