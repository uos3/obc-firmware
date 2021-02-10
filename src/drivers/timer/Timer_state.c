/**
 * @file Timer_interrupts.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides initial layout for TIMER_STATE global variable.
 * 
 * @version 0.1
 * @date 2021-02-03
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* System inclues */
#include <stdint.h>
#include <stdbool.h>

/* Tivaware includes */
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

/* Internal includes */
#include "drivers/timer/Timer_public.h"
#include "drivers/timer/Timer_private.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

Timer_State TIMER_STATE = {
    .blocks = {
        // BLOCK00
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_TIMER0,
            .base = TIMER0_BASE,
            .p_a = &TIMER_STATE.timers[0],
            .p_b = &TIMER_STATE.timers[1]
        },
        // BLOCK01
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_TIMER1,
            .base = TIMER1_BASE,
            .p_a = &TIMER_STATE.timers[2],
            .p_b = &TIMER_STATE.timers[3]
        },
        // BLOCK02
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_TIMER2,
            .base = TIMER2_BASE,
            .p_a = &TIMER_STATE.timers[4],
            .p_b = &TIMER_STATE.timers[5]
        },
        // BLOCK03
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_TIMER3,
            .base = TIMER3_BASE,
            .p_a = &TIMER_STATE.timers[6],
            .p_b = &TIMER_STATE.timers[7]
        },
        // BLOCK04
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_TIMER4,
            .base = TIMER4_BASE,
            .p_a = &TIMER_STATE.timers[8],
            .p_b = &TIMER_STATE.timers[9]
        },
        // BLOCK05
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_TIMER5,
            .base = WTIMER5_BASE,
            .p_a = &TIMER_STATE.timers[10],
            .p_b = &TIMER_STATE.timers[11]
        },
        // BLOCK06
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_WTIMER0,
            .base = WTIMER0_BASE,
            .p_a = &TIMER_STATE.timers[12],
            .p_b = &TIMER_STATE.timers[13]
        },
        // BLOCK07
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_WTIMER1,
            .base = WTIMER1_BASE,
            .p_a = &TIMER_STATE.timers[14],
            .p_b = &TIMER_STATE.timers[15]
        },
        // BLOCK08
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_WTIMER2,
            .base = WTIMER2_BASE,
            .p_a = &TIMER_STATE.timers[16],
            .p_b = &TIMER_STATE.timers[17]
        },
        // BLOCK09
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_WTIMER3,
            .base = WTIMER3_BASE,
            .p_a = &TIMER_STATE.timers[18],
            .p_b = &TIMER_STATE.timers[19]
        },
        // BLOCK10
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_WTIMER4,
            .base = WTIMER4_BASE,
            .p_a = &TIMER_STATE.timers[20],
            .p_b = &TIMER_STATE.timers[21]
        },
        // BLOCK11
        {
            .is_split = true,
            .config = 0,
            .peripheral = SYSCTL_PERIPH_WTIMER5,
            .base = WTIMER5_BASE,
            .p_a = &TIMER_STATE.timers[22],
            .p_b = &TIMER_STATE.timers[23]
        },
    },
    .timers = {
        // TIMER00A
        {
            .is_wide = false,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[0],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_00A_COMPLETE,
            .p_interrupt_handler = Timer_int_00A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER00B
        {
            .is_wide = false,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[0],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_00B_COMPLETE,
            .p_interrupt_handler = Timer_int_00B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER01A
        {
            .is_wide = false,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[1],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_01A_COMPLETE,
            .p_interrupt_handler = Timer_int_01A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER01B
        {
            .is_wide = false,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[1],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_01B_COMPLETE,
            .p_interrupt_handler = Timer_int_01B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER02A
        {
            .is_wide = false,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[2],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_02A_COMPLETE,
            .p_interrupt_handler = Timer_int_02A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER02B
        {
            .is_wide = false,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[2],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_02B_COMPLETE,
            .p_interrupt_handler = Timer_int_02B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER03A
        {
            .is_wide = false,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[3],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_03A_COMPLETE,
            .p_interrupt_handler = Timer_int_03A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER03B
        {
            .is_wide = false,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[3],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_03B_COMPLETE,
            .p_interrupt_handler = Timer_int_03B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER04A
        {
            .is_wide = false,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[4],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_04A_COMPLETE,
            .p_interrupt_handler = Timer_int_04A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER04B
        {
            .is_wide = false,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[4],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_04B_COMPLETE,
            .p_interrupt_handler = Timer_int_04B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER05A
        {
            .is_wide = false,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[5],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_05A_COMPLETE,
            .p_interrupt_handler = Timer_int_05A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER05B
        {
            .is_wide = false,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[5],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_05B_COMPLETE,
            .p_interrupt_handler = Timer_int_05B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER06A
        {
            .is_wide = true,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[6],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_06A_COMPLETE,
            .p_interrupt_handler = Timer_int_06A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER06B
        {
            .is_wide = true,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[6],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_06B_COMPLETE,
            .p_interrupt_handler = Timer_int_06B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER07A
        {
            .is_wide = true,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[7],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_07A_COMPLETE,
            .p_interrupt_handler = Timer_int_07A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER07B
        {
            .is_wide = true,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[7],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_07B_COMPLETE,
            .p_interrupt_handler = Timer_int_07B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER08A
        {
            .is_wide = true,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[8],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_08A_COMPLETE,
            .p_interrupt_handler = Timer_int_08A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER08B
        {
            .is_wide = true,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[8],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_08B_COMPLETE,
            .p_interrupt_handler = Timer_int_08B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER09A
        {
            .is_wide = true,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[9],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_09A_COMPLETE,
            .p_interrupt_handler = Timer_int_09A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER09B
        {
            .is_wide = true,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[9],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_09B_COMPLETE,
            .p_interrupt_handler = Timer_int_09B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER10A
        {
            .is_wide = true,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[10],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_10A_COMPLETE,
            .p_interrupt_handler = Timer_int_10A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER10B
        {
            .is_wide = true,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[10],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_10B_COMPLETE,
            .p_interrupt_handler = Timer_int_10B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER11A
        {
            .is_wide = true,
            .is_a = true,
            .p_block = &TIMER_STATE.blocks[11],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_11A_COMPLETE,
            .p_interrupt_handler = Timer_int_11A,
            .interrupt_mask = TIMER_TIMA_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
        // TIMER11B
        {
            .is_wide = true,
            .is_a = false,
            .p_block = &TIMER_STATE.blocks[11],
            .prescale = 0,
            .value = 0,
            .config = 0,
            .completed_event = EVT_TIMER_11B_COMPLETE,
            .p_interrupt_handler = Timer_int_11B,
            .interrupt_mask = TIMER_TIMB_TIMEOUT,
            .is_available = true,
            .is_periodic = false
        },
    },
    .p_next_16 = &TIMER_STATE.timers[0],
    .p_next_32 = &TIMER_STATE.timers[12],
    .p_next_64 = &TIMER_STATE.timers[12]
};