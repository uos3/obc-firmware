/**
 * @file Timer_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Private header for the timer driver.
 * 
 * Task ref: [UT_2.8.1]
 * 
 * @version 0.1
 * @date 2021-02-01
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_TIMER_PRIVATE_H
#define H_TIMER_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* System includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "drivers/timer/Timer_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Number of blocks (both single and double width) on the TM4C.
 */
#define TIMER_NUM_BLOCKS (12)

/**
 * @brief Numer of timers on the TM4C.
 */
#define TIMER_NUM_TIMERS (2*TIMER_NUM_BLOCKS)

/**
 * @brief Maximum number of times to check if a peripheral is ready to use.
 */
#define TIMER_MAX_NUM_PERIPH_READY_CHECKS (10)

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/* Pre-definition to allow Timer_Block to be used in Timer_Timer */
struct _Timer_Block;

typedef struct _Timer_Timer {
    /**
     * @brief True if the timer is a wide (32 bit) one.
     */
    bool is_wide;

    /**
     * @brief True if the timer is the A timer of it's block. Needed as config
     * constants in tivaware are split between A and B.
     */
    bool is_a;

    /**
     * @brief Pointer to the timer's owning block.
     */
    struct _Timer_Block *p_block;

    /**
     * @brief Prescale value used for the timer. The limit of this value is
     * based on the Timer's width:
     * 
     *  - not wide =  8 bit
     *  -     wide = 16 bit
     */
    uint16_t prescale;

    /**
     * @brief Value for the timer. The limit of this value is based on the
     * Timer's width and the owning block's split status:
     * 
     *  - not wide &&     split = 16 bit 
     *  -     wide &&     split = 32 bit
     *  - not wide && not split = 32 bit
     *  -     wide && not split = 64 bit
     */
    uint64_t value;

    /**
     * @brief Configuration for the timer
     */
    uint32_t config;

    /**
     * @brief Event to be fired when the timer completes.
     */
    Event completed_event;

    /**
     * @brief Pointer to the interrupt handler function for this timer.
     */
    void (*p_interrupt_handler)(void);

    /**
     * @brief Interrupt mask for this timer.
     */
    uint32_t interrupt_mask;

    /**
     * @brief True if the timer is free to be used
     */
    bool is_available;
    
    /**
     * @brief Whether the timer should be configured in periodic mode or not.
     */
    bool is_periodic;
} Timer_Timer;

typedef struct _Timer_Block {
    /**
     * @brief True if the timer block is split, i.e. both timers run
     * independently. 
     * 
     * If false Timer p_a holds all information and timer p_b will be ignored.
     */
    bool is_split;

    /**
     * @brief Configuration of the timer block.
     */
    uint32_t config;

    /**
     * @brief SysCtl peripheral of this block
     */
    uint32_t peripheral;

    /**
     * @brief Base address of this block
     */
    uint32_t base;

    /**
     * @brief Pointer to the A timer of this block
     */
    Timer_Timer *p_a;

    /**
     * @brief Pointer to the B timer of this block
     */
    Timer_Timer *p_b;
} Timer_Block;


/**
 * @brief Represents the state of all timer blocks
 */
typedef struct _Timer_State {

    /**
     * @brief All timer blocks
     */
    Timer_Block blocks[TIMER_NUM_BLOCKS];

    /**
     * @brief All timers
     */
    Timer_Timer timers[TIMER_NUM_TIMERS];

    /**
     * @brief Pointer to the next free 16 bit timer.
     */
    Timer_Timer *p_next_16;

    /**
     * @brief Pointer to the next free 32 bit timer.
     * 
     */
    Timer_Timer *p_next_32;

    /**
     * @brief Pointer to the  next free 64 bit timer.
     */
    Timer_Timer *p_next_64;

} Timer_State;

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief Global timer state
 */
extern Timer_State TIMER_STATE;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Configure a timer to use based on the input duration. 
 * 
 * This function will mutate the TIMER_STATE.
 * 
 * @param duration_s_in The duration the timer should be run for in seconds.
 * @param is_periodic_in True if the timer should be periodic, otherwise the
 * timer is one-shot.
 * @param p_event_out Pointer to the timer event that will signal completion.
 * @return ErrorCode Possible errors.
 */
ErrorCode Timer_configure_timer_for_duration(
    double duration_s_in,
    bool is_periodic_in,
    Event *p_event_out
);

/**
 * @brief Calculates the integer factors of the target number of cycles, 
 * accurate to within 1 of the target.
 * 
 * @param cycles_in Number of cycles to factorise.
 * @param max_prescale_value_in Maximum value that *p_prescale_out can have.
 * @param max_timer_value_in Maximum value that *p_timer_value_out can have.
 * @param p_prescale_out Value of the prescalar.
 * @param p_timer_value_out Value of the timer itself.
 * @return ErrorCode If no factor is found TIMER_ERROR_FACTOR_NOT_FOUND is
 * returned. 
 */
ErrorCode Timer_calc_timer_factors(
    uint64_t cycles_in,
    uint16_t max_prescale_value_in,
    uint64_t max_timer_value_in,
    uint16_t *p_prescale_out, 
    uint64_t *p_timer_value_out
);

/**
 * @brief Configures the given timer with the precale, value, and periodicity.
 * 
 * @param p_timer_in Timer to configure.
 * @param prescale_in Prescale value to use.
 * @param value_in Timer value to use.
 * @return ErrorCode If timer was NULL error will be returned.
 */
ErrorCode Timer_configure_timer(
    Timer_Timer *p_timer_in,
    uint16_t prescale_in,
    uint64_t value_in
);

/**
 * @brief Enables the given timer.
 * 
 * Timer must have been configured using Timer_configure_timer first.
 * 
 * @param p_timer_in Timer to configure.
 * @return ErrorCode If the timer couldn't be enabled an error is returne.
 */
ErrorCode Timer_enable(Timer_Timer *p_timer_in);

/**
 * @brief Updates internal pointers to the next available timers.
 * 
 * @return ErrorCode If an error occurs an error is returned.
 */
ErrorCode Timer_update_pointers(void);

/**
 * @brief Disables a specific timer.
 * 
 * @param p_timer_in Pointer to the timer 
 */
void Timer_disable_specific(Timer_Timer *p_timer_in);

/* -------------------------------------------------------------------------   
 * INTERRUPTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Used to raise an event during a timer interrupt
 * 
 * @param p_timer The timer who's event to raise
 */
void Timer_int_raise_event(Timer_Timer *p_timer);

void Timer_int_00A(void);
void Timer_int_00B(void);

void Timer_int_01A(void);
void Timer_int_01B(void);

void Timer_int_02A(void);
void Timer_int_02B(void);

void Timer_int_03A(void);
void Timer_int_03B(void);

void Timer_int_04A(void);
void Timer_int_04B(void);

void Timer_int_05A(void);
void Timer_int_05B(void);

void Timer_int_06A(void);
void Timer_int_06B(void);

void Timer_int_07A(void);
void Timer_int_07B(void);

void Timer_int_08A(void);
void Timer_int_08B(void);

void Timer_int_09A(void);
void Timer_int_09B(void);

void Timer_int_10A(void);
void Timer_int_10B(void);

void Timer_int_11A(void);
void Timer_int_11B(void);

#endif /* H_TIMER_PRIVATE_H */