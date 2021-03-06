/**
 * @file OpModeManager_events.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief OpModeManager event definitions
 * 
 * Task ref: [UT_2.9.10]
 * 
 * @version 0.1
 * @date 2021-03-08
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_OPMODEMANAGER_EVENTS_H
#define H_OPMODEMANAGER_EVENTS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * EVENTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Event raised when a change to the OPMODE is started.
 * 
 * OpMode changes take a non-zero amount of time to complete. Please see
 * SSS_OpModeTransition for more information.
 */
#define EVT_OPMODEMANAGER_OPMODE_CHANGE_STARTED ((Event)(MOD_ID_OPMODEMANAGER | 1))

/**
 * @brief Event raised when the OPMODE is changed.
 */
#define EVT_OPMODEMANAGER_OPMODE_CHANGED ((Event)(MOD_ID_OPMODEMANAGER | 2))

#endif /* H_OPMODEMANAGER_EVENTS_H */