/**
 * @ingroup event_manager
 * 
 * @file EventManager_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides private functions for the EventManager
 * 
 * @version 2.0
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_EVENTMANAGER_PRIVATE_H
#define H_EVENTMANAGER_PRIVATE_H


/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief The global EventManager instance
 */
extern EventManager EVENTMANAGER;

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief After this many cycles of being raised the event shall be considered
 * stale and will be polled from the raised event list.
 */
#define EVENTMANAGER_STALE_EVENT_CYCLE_THRESHOLD (2)

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief If the current number of raised events is less than 1/4 the allocated 
 * list size the lists shall be shrunk by 1/2.
 */
void EventManager_shrink_lists(void);

/**
 * @brief Remove stale events, i.e. those that have been raised for longer than
 * a threshold number of cycles.
 */
void EventManager_remove_stale_events(void);

#endif /* H_EVENTMANAGER_PRIVATE_H */