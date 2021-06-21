/**
 * @file Timer_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Implements the timer driver for the software.
 * 
 * Task ref: [UT_2.8.1]
 * 
 * The Timer driver provides abstractions over the TM4C's general purpose timer
 * peripherals. These abstractions allow timers to be used dynamically by the
 * software, so that consideration doesn't have to be given to which physical
 * timer perhipheral is being used for a particular module or task. 
 * 
 * The user can start a timer using the `Timer_start` functions, which return
 * an Event ID that the user can poll for. This event will be fired when the
 * timer completes. When a timer completes it becomes available for use by any
 * other part of the software.
 * 
 * The TM4C has a total of six single-width (32 bit) timer blocks and six
 * double-width (62 bit) timer blocks. Each block is split into an A and B
 * timer, so in a single width block there are two 16 bit times. These can be
 * used individually or can be combined into one 32 bit timer. The driver
 * handles management of this for the user, based on the duration passed into
 * the start function. 
 * 
 * @version 0.1
 * @date 2021-02-01
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_TIMER_PUBLIC_H
#define H_TIMER_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* Internal includes */
#include "drivers/timer/Timer_errors.h"
#include "drivers/timer/Timer_events.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the Timer driver
 * 
 * @return ErrorCode Possible errors during init.
 */
ErrorCode Timer_init(void);

/**
 * @brief Start a new single-shot timer of the given duration.
 * 
 * The width of the timer is selected based on the duration provided and the
 * available timers. The timer will be in down-counting single-shot mode (i.e.,
 * once the timer completes it will not restart). Use `Timer_start_periodic` to
 * start a timer that repeats.
 * 
 * @param duration_s_in The duration of the timer in seconds.
 * @param timer_event_out The event which will be fired when the timer
 * completes. 
 * @return ErrorCode Indicates any error starting the timer.
 */
ErrorCode Timer_start_one_shot(double duration_s_in, Event *p_timer_event_out);

/**
 * @brief Start a new periodic timer of the given duration.
 * 
 * The width of the timer is selected based on the duration provided and the
 * available timers. The timer will be in down-counting periodic mode (i.e.,
 * once the timer completes it will restart). Use `Timer_start_one_shot` to
 * start a timer that doesn't repeat.
 * 
 * @param duration_s_in The duration of the timer in seconds.
 * @param timer_event_out The event which will be fired when the timer
 * completes. 
 * @return ErrorCode Indicates any error starting the timer.
 */
ErrorCode Timer_start_periodic(double duration_s_in, Event *p_timer_event_out);

/**
 * @brief Disables the timer associated with the given timer event.
 * 
 * The user should pass in the event returned by Timer_start_x
 * 
 * @param timer_event_in Completed event for the timer to disable.
 * @return ErrorCode If the timer wasn't disabled an error is returned.
 */
ErrorCode Timer_disable(Event timer_event_in);

/**
 * @brief Disables all timers registered with the system.
 * 
 * CAUTION: This should only be used if you know what you're doing. General use
 * should use Timer_disable instead!
 */
void Timer_disable_all(void);

/* TODO: Add Timer_get_reserved? */

#endif /* H_TIMER_PUBLIC_H */