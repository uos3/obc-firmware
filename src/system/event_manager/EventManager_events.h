/**
 * @ingroup event_manager
 * 
 * @file EventManager_events.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides possible events for the EventManager.
 * 
 * @version 1.0
 * @date 2020-11-02
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_EVENTMANAGER_EVENTS_H
#define H_EVENTMANAGER_EVENTS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/event_manager/EventManager_public.h"
#include "system/event_manager/EventManager_blocks.h"
#include "drivers/i2c/I2c_events.h"
#include "components/imu/Imu_events.h"

/* -------------------------------------------------------------------------   
 * EVENTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Event associated with an event manager error.
 * 
 * This event indicates that an error has occured in the EventManager, and the
 * corresponding error code in the EventManager's DataPool
 * DP.EVENTMANAGER.ERROR_CODE has been set.
 * 
 * Note, if the error arises from already being at the maximum number of events
 * the error code will be set and the corresponding
 * DP.EVENTMANAGER.MAX_EVENTS_REACHED flag will be raised. This event will not
 * be raised.
 */
#define EVT_EVENTMANAGER_ERROR ((Event)(EVENTMANAGER_BLOCK_EVENTMANAGER | 1))


#endif /* H_EVENTMANAGER_EVENTS_H */