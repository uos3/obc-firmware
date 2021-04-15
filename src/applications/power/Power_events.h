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
 * @brief Event which signals that an OCP state change which was requested as a
 * part of an OPMODE transition has been completed.
 */
#define EVT_POWER_OPMODE_CHANGE_OCP_STATE_CHANGE_COMPLETE ((Event)(MOD_ID_POWER | 1))

/**
 * @brief Event signalling that an OCP reset operation has been successfully
 * completed. 
 */
#define EVT_POWER_OCP_RESET_SUCCESS ((Event)(MOD_ID_POWER | 2))

#endif /* H_POWER_EVENTS_H */