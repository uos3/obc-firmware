/**
 * @file Eps_events.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief EPS Component Module events definitions
 * 
 * Task ref: [UT_2.10.7]
 * 
 * @version 0.1
 * @date 2021-02-17
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_EPS_EVENTS_H
#define H_EPS_EVENTS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* Internal includes */
#include "system/kernel/Kernel_module_ids.h"
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * EVENTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Event that is fired when a command to the EPS is completed (sent and
 * recieved, or any failure during the send sequence).
 */
#define EVT_EPS_COMMAND_COMPLETE ((Event)(MOD_ID_EPS | 1))

/**
 * @brief Event which is fired when a new HK data packet is recieved from the
 * EPS. 
 * 
 * NOTE: Many consumers may be looking for this event, so the user must _NOT_
 * poll for it, instead use the EventManager_is_event_raised() function.
 */
#define EVT_EPS_NEW_HK_DATA ((Event)(MOD_ID_EPS | 2))

#endif /* H_EPS_EVENTS_H */