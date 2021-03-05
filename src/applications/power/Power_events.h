/**
 * @file Power_events.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application event code definitions
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-03-01
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_POWER_EVENTS_H
#define H_POWER_EVENTS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include "system/event_manager/EventManager_public.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * EVENTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Event which signals that the activities required for an OPMODE change
 * (for this app) are complete.
 */
#define EVT_POWER_OPMODE_CHANGE_ACTIVITIES_COMPLETE ((Event)(MOD_ID_POWER | 1))

#endif /* H_POWER_EVENTS_H */