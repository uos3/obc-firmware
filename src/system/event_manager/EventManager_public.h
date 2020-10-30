/**
 * @ingroup system
 * 
 * @file EventManager_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides EventManager system module.
 * 
 * The EventManager provides the following functionality:
 * - Raise events
 * - See if an event has been raised
 * - See if any events have been raised
 * - Clear events that have been raised
 * - Single global instance of the EventManager
 * - Events raised for two or more cycles will be discarded. Events shall be
 *   cleared within two cycles so that unserviced event accumulation does not
 *   happen, and prevent exceeding the maximum number of raised events
 *   allowable at one time. 
 * TODO: More detailed reasoning behind 2 cycles.
 * 
 * @version 0.1
 * @date 2020-10-28
 * 
 * @copyright Copyright (c) UoS3 2020
 * 
 * @defgroup event_manager Event Manager
 * @{
 */

#ifndef H_EVENTMANAGER_PUBLIC_H
#define H_EVENTMANAGER_PUBLIC_H

/* -------------------------------------------------------------------------
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */


/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The maximum size of the lists in the EventManager.
 * 
 * Implicitly this is also the limit on the number of events that can be raised
 * at any one time.
 * 
 * TODO: This value is currently arbitrary. 
 */
#define EVENTMANAGER_MAX_LIST_SIZE (256)

/**
 * @brief The minimum size of the lists in the EventManager.
 */
#define EVENTMANAGER_MIN_LIST_SIZE (8)

/**
 * @brief The multipler used to determine if the lists need to be shrunk.
 * 
 * If (num raised events) * (this multiplier) <= (list size) then the lists 
 * will be shrunk.
 */
#define EVENTMANAGER_SHRINK_LIST_MULTIPLIER (4)

/**
 * @brief The divisor to use to calculate the new size of the lists when
 * shrinking.
 * 
 * The new shrunk list size will be (list size) / (this divisor).
 */
#define EVENTMANAGER_SHRUNK_LIST_SIZE_DIVISOR (2)

/* -------------------------------------------------------------------------   
 * TYPES
 * ------------------------------------------------------------------------- */

/**
 * @brief The underlying type for events.
 * 
 * Events are 16 bit as it is not expected to have more than 65536 types of 
 * event, and due to memory constraints it is preferable to use the smallest
 * possible integer type. It is expected there will be more than 256 events,
 * therefore 16 bit is the smallest that can be used.
 */
typedef uint16_t Event;

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief The EventManagerstructure.
 * 
 * The EventManager is responsible for controlling the use of events by the
 * code base. It provides a global interface for the raising, checking, and
 * polling of events via the EventManager_x functions.
 * 
 * List allocation strategy:
 *  - When the current number of raised events is 1 less than the size of the
 *    list the size of the lists will be doubled, unless this would exceed the
 *    maximum size of the list, in which case an error will occur.
 *  - When the current number of raised events is less than 1/4 of the size of
 *    the lists the lists will be shrunk to 1/2 their current size, unless this
 *    would make the list smaller than the minimum size.
 * 
 * This is done to minimise the number of allocations/deallocs required when
 * raising or polling events.
 */
typedef struct _EventManager {
    /**
     * @brief Flag which is true when the EventManager has been initialised.
     */
    bool initialised;

    /**
     * @brief A pointer to the dynamically allocated list of raised events.
     */
    Event *p_raised_events;

    /**
     * @brief A pointer to the dynamically allocated list of the number of
     * cycles that an event has been raised for. Used in event clearup.
     */
    uint8_t *p_num_cycles_events_raised;

    /**
     * @brief The number of currently raised events.
     */
    uint8_t num_raised_events;

    /**
     * @brief The current size of the dynamically allocated lists.
     */
    size_t size_of_lists;
} EventManager;

// TODO: REMOVE
extern EventManager EVENTMANAGER;

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
bool EventManager_shrink_lists();

/**
 * @brief Initialise the event manager to it's default state.
 * 
 * @return true Initialisation succeeded.
 * @return false Initialisation failed.
 */
bool EventManager_init();

/**
 * @brief Destroy the event manager by deallocating all memory associated with
 * it.
 */
void EventManager_destroy();

/**
 * @brief Raise the given event.
 * 
 * @param event_in The event to raise.
 * @return true The event was successfully raised.
 * @return false The event could not be raised.
 */
bool EventManager_raise_event(Event event_in);

/**
 * @brief Check if an event has been raised. The event is not cleared if it is
 * raised. 
 * 
 * Use EventManager_poll_event to check if the event is raised and clear it.
 * 
 * @param event_in The event to check.
 * @param p_is_raised_out true if raised, false if not.
 * @return true The function executed successfully.
 * @return false The function failed to execute.
 */
bool EventManager_is_event_raised(Event event_in, bool *p_is_raised_out);

/**
 * @brief Poll an event to see if it is raised. If the event is raised clear
 * it. 
 * 
 * Use EventManager_is_event_raised to check if an event is raised and not
 * clear it.
 * 
 * @param event_in The event to check.
 * @param p_is_raised_out true if raised, false if not.
 * @return true The function executed successfully.
 * @return false The function failed to execute.
 */
bool EventManager_poll_event(Event event_in, bool *p_is_raised_out);

/**
 * @brief Clear all the events in the manager.
 * 
 * @return true All events were cleared.
 * @return false An error occured.
 */
bool EventManager_clear_all_events();

/**
 * @brief Get the number of currently raised events.
 * 
 * @param p_num_raised_events_out Pointer to the number of raised events.
 * @return true The function executed successfully.
 * @return false The function failed to execute.
 */
bool EventManager_get_num_raised_events(uint8_t *p_num_raised_events_out);

/**
 * @brief Clean up events that have been raised for two or more cycles.
 * 
 * This function should be executed at the end of the software cycle.
 * 
 * @return true Cleanup executed successfully.
 * @return false Cleanup failed.
 */
bool EventManager_cleanup_events();

#endif /* H_EVENTMANAGER_PUBLIC_H */

/** @} */ /* End of event_manager */