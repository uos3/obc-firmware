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

#define EVT_TIMER_00A_COMPLETE ((Event)(MOD_ID_TIMER | 10))
#define EVT_TIMER_00B_COMPLETE ((Event)(MOD_ID_TIMER | 11))

#define EVT_TIMER_01A_COMPLETE ((Event)(MOD_ID_TIMER | 12))
#define EVT_TIMER_01B_COMPLETE ((Event)(MOD_ID_TIMER | 13))

#define EVT_TIMER_02A_COMPLETE ((Event)(MOD_ID_TIMER | 14))
#define EVT_TIMER_02B_COMPLETE ((Event)(MOD_ID_TIMER | 15))

#define EVT_TIMER_03A_COMPLETE ((Event)(MOD_ID_TIMER | 16))
#define EVT_TIMER_03B_COMPLETE ((Event)(MOD_ID_TIMER | 17))

#define EVT_TIMER_04A_COMPLETE ((Event)(MOD_ID_TIMER | 18))
#define EVT_TIMER_04B_COMPLETE ((Event)(MOD_ID_TIMER | 19))

#define EVT_TIMER_05A_COMPLETE ((Event)(MOD_ID_TIMER | 20))
#define EVT_TIMER_05B_COMPLETE ((Event)(MOD_ID_TIMER | 21))

#define EVT_TIMER_06A_COMPLETE ((Event)(MOD_ID_TIMER | 22))
#define EVT_TIMER_06B_COMPLETE ((Event)(MOD_ID_TIMER | 23))

#define EVT_TIMER_07A_COMPLETE ((Event)(MOD_ID_TIMER | 24))
#define EVT_TIMER_07B_COMPLETE ((Event)(MOD_ID_TIMER | 25))

#define EVT_TIMER_08A_COMPLETE ((Event)(MOD_ID_TIMER | 26))
#define EVT_TIMER_08B_COMPLETE ((Event)(MOD_ID_TIMER | 27))

#define EVT_TIMER_09A_COMPLETE ((Event)(MOD_ID_TIMER | 28))
#define EVT_TIMER_09B_COMPLETE ((Event)(MOD_ID_TIMER | 29))

#define EVT_TIMER_10A_COMPLETE ((Event)(MOD_ID_TIMER | 30))
#define EVT_TIMER_10B_COMPLETE ((Event)(MOD_ID_TIMER | 31))

#define EVT_TIMER_11A_COMPLETE ((Event)(MOD_ID_TIMER | 32))
#define EVT_TIMER_11B_COMPLETE ((Event)(MOD_ID_TIMER | 33))

#endif /* H_TIMER_EVENTS_H */