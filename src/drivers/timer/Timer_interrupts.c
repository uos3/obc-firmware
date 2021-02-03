/**
 * @file Timer_interrupts.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides interrupts for timers.
 * 
 * @version 0.1
 * @date 2021-02-03
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdbool.h>
#include <stdint.h>

/* Tivaware includes */
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/timer/Timer_private.h"
#include "drivers/timer/Timer_events.h"

/* -------------------------------------------------------------------------   
 * INTERRUPT HANDLERS
 * ------------------------------------------------------------------------- */

void Timer_int_raise_event(Timer_Timer *p_timer) {
    /* Disable interrupts to prevent another one from triggering any changes to
     * the event manager too */
    IntMasterDisable();

    /* Clear the interrupt. This must be done early as it can take a number of
     * clock cycles for the clear to have an effect. */
    TimerIntClear(
        p_timer->p_block->base,
        p_timer->interrupt_mask
    );

    /* Raise the event.
     * TODO: Error handling in interrupt? */
    EventManager_raise_event(p_timer->completed_event);

    /* If the timer is not periodic make it available. If it was configured 
     * joined with it's other pair enable the pair too */
    if (!p_timer->is_periodic) {
        p_timer->is_available = true;

        if (!p_timer->p_block->is_split) {
            p_timer->p_block->p_b->is_available = true;
        }
    }

    /* Reenable interrupts */
    IntMasterEnable();
}

void Timer_int_00A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[0].p_a);    
}

void Timer_int_00B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[0].p_b);
}

void Timer_int_01A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[1].p_a);    
}

void Timer_int_01B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[1].p_b);
}

void Timer_int_02A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[2].p_a);    
}

void Timer_int_02B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[2].p_b);
}

void Timer_int_03A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[3].p_a);    
}

void Timer_int_03B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[3].p_b);
}

void Timer_int_04A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[4].p_a);    
}

void Timer_int_04B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[4].p_b);
}

void Timer_int_05A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[5].p_a);    
}

void Timer_int_05B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[5].p_b);
}

void Timer_int_06A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[6].p_a);    
}

void Timer_int_06B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[6].p_b);
}

void Timer_int_07A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[7].p_a);    
}

void Timer_int_07B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[7].p_b);
}

void Timer_int_08A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[8].p_a);    
}

void Timer_int_08B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[8].p_b);
}

void Timer_int_09A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[9].p_a);    
}

void Timer_int_09B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[9].p_b);
}

void Timer_int_10A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[10].p_a);    
}

void Timer_int_10B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[10].p_b);
}

void Timer_int_11A(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[11].p_a);    
}

void Timer_int_11B(void) {
    Timer_int_raise_event(TIMER_STATE.blocks[11].p_b);
}