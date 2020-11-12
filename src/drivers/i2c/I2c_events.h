/**
 * @ingroup i2c
 * 
 * @file I2c_events.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Defines the events available for the I2C module.
 * 
 * @version 0.1
 * @date 2020-11-12
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_I2C_EVENTS_H
#define H_I2C_EVENTS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/event_manager/EventManager_public.h"
#include "system/event_manager/EventManager_blocks.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief This event indicates that a new action has been requested for the I2C
 * driver to execute.
 */
#define EVT_I2C_NEW_ACTION (EVENTMANAGER_BLOCK_I2C | 1)

/**
 * @brief Event that indicates when an I2C action has been finished. The user
 * must use I2c_get_device_action_status for their device to determine whether
 * or not their action has finished, or if it was another action.
 * 
 * This event __shall not__ be polled by the user, instead
 * EventManager_is_event_raised must be used. The I2c module will clear the
 * event when all finished actions have been handled.
 */
#define EVT_I2C_ACTION_FINISHED (EVENTMANAGER_BLOCK_I2C | 2)

#endif /* H_I2C_EVENTS_H */