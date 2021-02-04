/**
 * @file Timer_events.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Defines events for the Timer driver.
 * 
 * Task ref: [UT_2.8.1]
 * 
 * @version 0.1
 * @date 2021-02-01
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_TIMER_EVENTS_H
#define H_TIMER_EVENTS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_module_ids.h"
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * EVENTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Event fired when timer 00A completes.
 */
#define EVT_TIMER_00A_COMPLETE ((Event)(MOD_ID_TIMER | 10))
/**
 * @brief Event fired when timer 00B completes.
 */
#define EVT_TIMER_00B_COMPLETE ((Event)(MOD_ID_TIMER | 11))

/**
 * @brief Event fired when timer 01A completes.
 */
#define EVT_TIMER_01A_COMPLETE ((Event)(MOD_ID_TIMER | 12))
/**
 * @brief Event fired when timer 01B completes.
 */
#define EVT_TIMER_01B_COMPLETE ((Event)(MOD_ID_TIMER | 13))

/**
 * @brief Event fired when timer 02A completes.
 */
#define EVT_TIMER_02A_COMPLETE ((Event)(MOD_ID_TIMER | 14))
/**
 * @brief Event fired when timer 02B completes.
 */
#define EVT_TIMER_02B_COMPLETE ((Event)(MOD_ID_TIMER | 15))

/**
 * @brief Event fired when timer 03A completes.
 */
#define EVT_TIMER_03A_COMPLETE ((Event)(MOD_ID_TIMER | 16))
/**
 * @brief Event fired when timer 03B completes.
 */
#define EVT_TIMER_03B_COMPLETE ((Event)(MOD_ID_TIMER | 17))

/**
 * @brief Event fired when timer 04A completes.
 */
#define EVT_TIMER_04A_COMPLETE ((Event)(MOD_ID_TIMER | 18))
/**
 * @brief Event fired when timer 04B completes.
 */
#define EVT_TIMER_04B_COMPLETE ((Event)(MOD_ID_TIMER | 19))

/**
 * @brief Event fired when timer 05A completes.
 */
#define EVT_TIMER_05A_COMPLETE ((Event)(MOD_ID_TIMER | 20))
/**
 * @brief Event fired when timer 05B completes.
 */
#define EVT_TIMER_05B_COMPLETE ((Event)(MOD_ID_TIMER | 21))

/**
 * @brief Event fired when timer 06A completes.
 */
#define EVT_TIMER_06A_COMPLETE ((Event)(MOD_ID_TIMER | 22))
/**
 * @brief Event fired when timer 06B completes.
 */
#define EVT_TIMER_06B_COMPLETE ((Event)(MOD_ID_TIMER | 23))

/**
 * @brief Event fired when timer 07A completes.
 */
#define EVT_TIMER_07A_COMPLETE ((Event)(MOD_ID_TIMER | 24))
/**
 * @brief Event fired when timer 07B completes.
 */
#define EVT_TIMER_07B_COMPLETE ((Event)(MOD_ID_TIMER | 25))

/**
 * @brief Event fired when timer 08A completes.
 */
#define EVT_TIMER_08A_COMPLETE ((Event)(MOD_ID_TIMER | 26))
/**
 * @brief Event fired when timer 08B completes.
 */
#define EVT_TIMER_08B_COMPLETE ((Event)(MOD_ID_TIMER | 27))

/**
 * @brief Event fired when timer 09A completes.
 */
#define EVT_TIMER_09A_COMPLETE ((Event)(MOD_ID_TIMER | 28))
/**
 * @brief Event fired when timer 09B completes.
 */
#define EVT_TIMER_09B_COMPLETE ((Event)(MOD_ID_TIMER | 29))

/**
 * @brief Event fired when timer 10A completes.
 */
#define EVT_TIMER_10A_COMPLETE ((Event)(MOD_ID_TIMER | 30))
/**
 * @brief Event fired when timer 10B completes.
 */
#define EVT_TIMER_10B_COMPLETE ((Event)(MOD_ID_TIMER | 31))

/**
 * @brief Event fired when timer 11A completes.
 */
#define EVT_TIMER_11A_COMPLETE ((Event)(MOD_ID_TIMER | 32))
/**
 * @brief Event fired when timer 11B completes.
 */
#define EVT_TIMER_11B_COMPLETE ((Event)(MOD_ID_TIMER | 33))

#endif /* H_TIMER_EVENTS_H */