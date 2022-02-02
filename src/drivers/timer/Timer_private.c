/**
 * @file Timer_private.c
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

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* System inclues */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

/* Tivaware includes */
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/timer/Timer_public.h"
#include "drivers/timer/Timer_private.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

bool TIMER_MODULE_DISABLED = true;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Timer_configure_timer_for_duration(
    double duration_s_in,
    bool is_periodic_in,
    Event *p_event_out
) {
    uint16_t prescale;
    uint64_t timer_value;
    uint64_t target_cycles;
    Timer_Timer *p_timer;
    ErrorCode error = ERROR_NONE;

    /* Get the clock frequency */
    uint32_t clock_freq_hz = SysCtlClockGet();

    /* The prescale period is the time that a prescale-sized (8 bit) counter
     * running at the clock frequency would give. This can then be multiplied 
     * by the maximum value of each timer's size to get the maximum period that
     * the timer can measure. Note that a prescaler value of 0 corresponds to a
     * 1 multiplier, so we take 2^8 not 2^8 - 1 as the max value. */
    double max_prescale_period_s = 256.0/((double)clock_freq_hz);

    /* Calcualte number of target cycles. This is the number of clock cycles
     * required to get the requested duration. */
    target_cycles = (uint64_t)lround(
        duration_s_in * ((double)clock_freq_hz)
    );

    /* Decide which size of timer to use. We want to maximize the numeber of
     * timers available, which means using the smallest timer possible. */
    if (max_prescale_period_s * ((double)UINT16_MAX) >= duration_s_in) {

        /* Check there is a 16 bit timer available */
        if (TIMER_STATE.p_next_16 == NULL) {
            DEBUG_ERR("Duration requires 16 bit timer but none is available");
            return TIMER_ERROR_NO_16_BIT_TIMERS;
        }

        /* Calculate the factors needed */
        error = Timer_calc_timer_factors(
            target_cycles,
            UINT8_MAX,
            UINT16_MAX,
            &prescale,
            &timer_value
        );

        if (error != ERROR_NONE) {
            DEBUG_ERR("Couldn't find prescale and timer values");
            return error;
        }

        /* Set the timer to use */
        p_timer = TIMER_STATE.p_next_16;

        /* Configure the timer's block */
        p_timer->p_block->is_split = true;
        p_timer->p_block->config = TIMER_CFG_SPLIT_PAIR;
    }
    /* If the next 32 bit is a combined one (i.e. the timer is not wide), 
     * there's no prescale available, so when checking the number of cycles 
     * don't include the prescale. */
    else if (!TIMER_STATE.p_next_32->is_wide
        &&
        (double)UINT32_MAX >= duration_s_in
    ) {

        /* Check there is a 32 bit timer available */
        if (TIMER_STATE.p_next_32 == NULL) {
            DEBUG_ERR("Duration requires 32 bit timer but none is available");
            return TIMER_ERROR_NO_32_BIT_TIMERS;
        }

        /* Calculate the factors needed, noting no prescale as this is a
         * combined timer */
        error = Timer_calc_timer_factors(
            target_cycles,
            0,
            UINT32_MAX,
            &prescale,
            &timer_value
        );

        if (error != ERROR_NONE) {
            DEBUG_ERR("Couldn't find prescale and timer values");
            return error;
        }

        /* Set the timer to use */
        p_timer = TIMER_STATE.p_next_32;

        /* If the next 32 bit timer is wide configure the block to be split */
        if (p_timer->is_wide) {
            p_timer->p_block->is_split = true;
            p_timer->p_block->config = TIMER_CFG_SPLIT_PAIR;
        }
        /* Otherwise set the block to be a joined and periodic based on input */
        else {
            p_timer->p_block->is_split = false;
            if (is_periodic_in) {
                p_timer->p_block->config = TIMER_CFG_PERIODIC;
            }
            else {
                p_timer->p_block->config = TIMER_CFG_ONE_SHOT;
            }
        }
    }
    /* Or if the next 32 bit timer is wide we can use a prescaler */
    else if (TIMER_STATE.p_next_32->is_wide
        &&
        (max_prescale_period_s * (double)UINT32_MAX) >= duration_s_in
    ) {

        /* Check there is a 32 bit timer available */
        if (TIMER_STATE.p_next_32 == NULL) {
            DEBUG_ERR("Duration requires 32 bit timer but none is available");
            return TIMER_ERROR_NO_32_BIT_TIMERS;
        }

        /* Calculate the factors needed, noting the 16 bit prescale for wide
         * timers.  */
        error = Timer_calc_timer_factors(
            target_cycles,
            UINT16_MAX,
            UINT32_MAX,
            &prescale,
            &timer_value
        );

        if (error != ERROR_NONE) {
            DEBUG_ERR("Couldn't find prescale and timer values");
            return error;
        }

        /* Set the timer to use */
        p_timer = TIMER_STATE.p_next_32;

        /* If the next 32 bit timer is wide configure the block to be split */
        if (p_timer->is_wide) {
            p_timer->p_block->is_split = true;
            p_timer->p_block->config = TIMER_CFG_SPLIT_PAIR;
        }
        /* Otherwise set the block to be a joined and periodic based on input */
        else {
            p_timer->p_block->is_split = false;
            if (is_periodic_in) {
                p_timer->p_block->config = TIMER_CFG_PERIODIC;
            }
            else {
                p_timer->p_block->config = TIMER_CFG_ONE_SHOT;
            }
        }
    }
    /* Finally for 64 bit timers no prescale is available (they're a
     * combination of two 32 bit wides.) */
    else if ((double)UINT64_MAX >= duration_s_in) {

        /* Check there is a 64 bit timer available */
        if (TIMER_STATE.p_next_64 == NULL) {
            DEBUG_ERR("Duration requires 64 bit timer but none is available");
            return TIMER_ERROR_NO_64_BIT_TIMERS;
        }

        /* Calculate the factors needed */
        error = Timer_calc_timer_factors(
            target_cycles,
            0,
            UINT64_MAX,
            &prescale,
            &timer_value
        );

        if (error != ERROR_NONE) {
            DEBUG_ERR("Couldn't find prescale and timer values");
            return error;
        }

        /* Set the timer to use */
        p_timer = TIMER_STATE.p_next_64;
        
        /* Configure the timer's block to be joined and periodic on input */
        p_timer->p_block->is_split = false;
        if (is_periodic_in) {
            p_timer->p_block->config = TIMER_CFG_PERIODIC;
        }
        else {
            p_timer->p_block->config = TIMER_CFG_ONE_SHOT;
        }
    }
    /* If the period cannot fit in a timer return an error */
    else {
        DEBUG_ERR(
            "Cannot start timer of duration %f s, as the maximum is %f",
            duration_s_in,
            max_prescale_period_s * ((double)UINT64_MAX)
        );
        return TIMER_ERROR_DURATION_TOO_LARGE;
    }

    /* Set the timer's periodicity */
    p_timer->is_periodic = is_periodic_in;

    /* Set the timer's config */
    error = Timer_configure_timer(
        p_timer,
        prescale,
        timer_value
    );

    if (error != ERROR_NONE) {
        DEBUG_ERR("No available timers");
        return error;
    }

    /* Enable the timer. This will also update the timer pointers. */
    error = Timer_enable(p_timer);

    if (error != ERROR_NONE) {
        DEBUG_ERR("Couldn't enable timer");
        return error;
    }

    /* Set the event */
    *p_event_out = p_timer->completed_event;

    return ERROR_NONE;
}

ErrorCode Timer_calc_timer_factors(
    uint64_t cycles_in,
    uint16_t max_prescale_value_in,
    uint64_t max_timer_value_in,
    uint16_t *p_prescale_out, 
    uint64_t *p_timer_value_out
) {
    uint16_t min_prescale;
    int64_t target_diff;
    uint16_t prescale;
    uint64_t timer_value;
    uint16_t prescale_min_error = 0;
    uint64_t value_min_error = 0;
    uint64_t min_error = UINT64_MAX;

    /* Minimum possible value of the prescalar, based on the maximum timer
     * value that is allowed. Previous checks shall ensure this value won't
     * overflow. */
    min_prescale = (uint16_t)lround(
        (double)cycles_in / (double)max_timer_value_in
    ) + 1;

    /* Limit min prescale to max value */
    if (min_prescale > max_prescale_value_in) {
        min_prescale = max_prescale_value_in;
    }

    /* If the minimum prescale is less than one, must only use the timer value
     * itself to get the target */
    if (min_prescale <= 1) {
        *p_timer_value_out = cycles_in;
        *p_prescale_out = 0;

        /* Limit to maximum timer value */
        if (*p_timer_value_out > max_timer_value_in) {
            *p_timer_value_out = max_timer_value_in;
        }

        /* Assert that the factors are within 1 of the target */
        target_diff = (int64_t)(*p_timer_value_out) - (int64_t)cycles_in;
        if (target_diff * target_diff > 1) {
            DEBUG_ERR(
                "Timer factor error is %d cycles (must be |diff| < 1) (no prescale available, timer too small)", 
                target_diff
            );
            return TIMER_ERROR_NO_FACTORS_FOUND;
        }
    }
    /* If the min_prescale would lead to an exact value when divided, use it */
    else if (cycles_in % min_prescale == 0) {
        /* Note that the prescaler is 1 based, so in our maths where prescaler
         * can't be zero we must subtract 1 to get the prescale value to give 
         * to the diverlib. (i.e. a prescalar of 0 in the driverlib is actually
         * a prescalar multiplier of 1. */
        *p_prescale_out = (uint16_t)(min_prescale - 1);
        *p_timer_value_out = lround((double)cycles_in / (double)min_prescale);

        /* Assert that the factors are within 1 of the target */
        target_diff = (min_prescale * *p_timer_value_out) - cycles_in;
        if (target_diff * target_diff > 1) {
            DEBUG_ERR(
                "Timer factor error is %d cycles (must be |diff| < 1)", 
                target_diff
            );
            return TIMER_ERROR_NO_FACTORS_FOUND;
        }
    }
    else {
        /* For all possible values of the prescaler */
        for (
            prescale = min_prescale; 
            prescale < max_prescale_value_in; 
            ++prescale
        ) {

            /* Calculate the corresponding target value needed for this
             * prescalar to get the target number of cycles. We round to reduce
             * the error as much as possible, as floor or ceil would incurr
             * greater error as you cross a boundary. */
            timer_value = (uint64_t)lround(
                (double)cycles_in / (double)prescale
            );

            /* Calcualte the difference between the combination of timer and
             * prescalar value and the target number of cycles */
            target_diff = (timer_value * prescale) - cycles_in;

            /* If the error (diff^2) is less than the current minimum error
             * set these prescale/timer values to be the best */
            if (target_diff * target_diff < min_error) {
                min_error = target_diff * target_diff;
                prescale_min_error = prescale;
                value_min_error = timer_value;
            }

            /* If the error is less than 1 we accept this as a good result */
            if (target_diff * target_diff < 1) {
                break;
            }
        }

        /* Set the prescale and timer values that produced the minimum error,
         * noting the same -1 for prescale as above */
        *p_prescale_out = (uint16_t)(prescale_min_error - 1);
        *p_timer_value_out = value_min_error;

        /* Assert that the factors are within 1 of the target */
        target_diff = (prescale_min_error * value_min_error) - cycles_in;
        if (target_diff * target_diff > 1) {
            DEBUG_ERR(
                "Timer factor error is %d cycles (must be |diff| < 1)", 
                target_diff
            );
            return TIMER_ERROR_NO_FACTORS_FOUND;
        }
    }

    return ERROR_NONE;
}

ErrorCode Timer_configure_timer(
    Timer_Timer *p_timer_in,
    uint16_t prescale_in,
    uint64_t value_in
) {
    /* Check for NULL */
    if (p_timer_in == NULL) {
        DEBUG_ERR("Recieved NULL timer as input");
        return TIMER_ERROR_NULL_TIMER;
    }

    /* Set the prescale and value */
    p_timer_in->prescale = prescale_in;
    p_timer_in->value = value_in;

    /* If the timer is a part of a joined block zero it's config. The config is
     * controlled by the block. */
    if (!p_timer_in->p_block->is_split) {
        p_timer_in->config = 0;
        p_timer_in->p_block->p_b->config = 0;
        return ERROR_NONE;
    }

    /* Set the config, being careful to use the right config for A or B timers. */
    if (p_timer_in->is_periodic && p_timer_in->is_a) {
        p_timer_in->config = TIMER_CFG_A_PERIODIC;
    }
    else if (p_timer_in->is_periodic && !p_timer_in->is_a) {
        p_timer_in->config = TIMER_CFG_B_PERIODIC;
    }
    else if (!p_timer_in->is_periodic && p_timer_in->is_a) {
        p_timer_in->config = TIMER_CFG_A_ONE_SHOT;
    }
    else if (!p_timer_in->is_periodic && !p_timer_in->is_a) {
        p_timer_in->config = TIMER_CFG_B_ONE_SHOT;
    }

    return ERROR_NONE;
}

ErrorCode Timer_enable(Timer_Timer *p_timer_in) {
    Timer_Timer *p_other;
    
    /* Check for NULL */
    if (p_timer_in == NULL) {
        DEBUG_ERR("Recieved NULL timer as input");
        return TIMER_ERROR_NULL_TIMER;
    }

    /* Get pointer to the other timer in the block */
    if (p_timer_in->is_a) {
        p_other = p_timer_in->p_block->p_b;
    }
    else {
        p_other = p_timer_in->p_block->p_a;
    }

    /* Configure the timer block */
    TimerConfigure(
        p_timer_in->p_block->base,
        (p_timer_in->p_block->config | p_timer_in->config | p_other->config)
    );

    /* Load the value for this timer. If a 64 bit timer use the set64 function
     * instead of the normal set function. */
    if (p_timer_in->is_wide && !p_timer_in->p_block->is_split) {
        TimerLoadSet64(p_timer_in->p_block->base, p_timer_in->value);
    }
    else if (p_timer_in->is_a) {
        TimerLoadSet(
            p_timer_in->p_block->base, 
            TIMER_A, 
            (uint32_t)p_timer_in->value
        );
    }
    else {
        TimerLoadSet(
            p_timer_in->p_block->base, 
            TIMER_B, 
            (uint32_t)p_timer_in->value
        );
    }

    /* Load the prescale value */
    if (p_timer_in->is_a) {
        TimerPrescaleSet(
            p_timer_in->p_block->base, 
            TIMER_A, 
            p_timer_in->prescale
        );
    }
    else {
        TimerPrescaleSet(
            p_timer_in->p_block->base,
            TIMER_B,
            p_timer_in->prescale
        );
    }

    /* Enable the timer's interrupt */
    TimerIntEnable(
        p_timer_in->p_block->base,
        (p_timer_in->interrupt_mask | p_other->interrupt_mask)
    );

    /* Enable the timer. If it's a combined block, or the other time is already
     * running, enable both. Otherwise just enable this one */
    if (!p_timer_in->p_block->is_split
        ||
        (p_timer_in->p_block->is_split && !p_other->is_available)
    ) {
        TimerEnable(p_timer_in->p_block->base, TIMER_BOTH);
    }
    else if (p_timer_in->is_a) {
        TimerEnable(p_timer_in->p_block->base, TIMER_A);
    }
    else {
        TimerEnable(p_timer_in->p_block->base, TIMER_B);
    }

    /* Mark this timer as not available */
    p_timer_in->is_available = false;

    /* If the block isn't split also mark the other as not available */
    if (!p_timer_in->p_block->is_split) {
        p_other->is_available = false;
    }

    /* Update timer pointers */
    return Timer_update_pointers();
}

ErrorCode Timer_update_pointers(void) {
    Timer_Timer *p_next;
    Timer_Timer *p_first_timer = &TIMER_STATE.timers[0];
    Timer_Timer *p_last_timer = &TIMER_STATE.timers[TIMER_NUM_TIMERS - 1];
    bool next_timer_found = false;

    /* FIXME: Potential bug here where timers are all used up when some are
     * actually available? */

    /* General update algorithm:
     *  - If current available pointer is available do nothing
     *  - Otherwise:
     *      - If the next timer is the right type, increment and check
     *        availability again.
     *      - If not, search all timers matching that type:
     *          - If one is found change the current free to that one
     *          - If none is found mark the current as NULL and warn that we
     *            are out of timers of this type.
     * 
     * Note that not finding a timer isn't an error, it's only an error if
     * we attempt to create a new timer when there isn't one.
     */

    /* 16-bit timer update */
    while (TIMER_STATE.p_next_16 != NULL
        &&
        !TIMER_STATE.p_next_16->is_available
        &&
        !next_timer_found
    ) {
        /* Get next pointer */
        p_next = TIMER_STATE.p_next_16;

        /* If next is of the right type increment the pointer (and we're in the
         * array still) */
        if (p_next <= p_last_timer && !p_next->is_wide) {
            TIMER_STATE.p_next_16 = p_next;
        }
        /* If not the right type, loop from the first timer to the last trying
         * to find one that will work */
        else {
            for (p_next = p_first_timer; p_next < p_last_timer; ++p_next) {
                /* If the right type and available, set the state and set that
                 * the next timer has been found, exit loop */
                if (!p_next->is_wide && p_next->is_available) {
                    TIMER_STATE.p_next_16 = p_next;
                    next_timer_found = true;
                    break;
                }
            }

            /* If we got through the loop without finding a timer there isn't
             * one left */
            TIMER_STATE.p_next_16 = NULL;
            DEBUG_WRN("All 16 bit timers in use");
        }
    }

    /* 32 bit timer update */
    next_timer_found = false;
    while (TIMER_STATE.p_next_32 != NULL
        &&
        !next_timer_found
    ) {
        /* If the current 32 bit (either wide or short) is available no need to
         * increment */
        if (TIMER_STATE.p_next_32->is_wide 
            && 
            TIMER_STATE.p_next_32->is_available
        ) {
            break;
        }
        if (!TIMER_STATE.p_next_32->is_wide
            &&
            TIMER_STATE.p_next_32->is_available
            &&
            (TIMER_STATE.p_next_32 + 1)->is_available
        ) {
            break;
        }

        /* As 32 bit timers can be build either from two shorts or two wides,
         * we need to check if the next two timers are both wide (i.e. only
         * move one up), otherwise we must move two up. If we are within 2 of
         * the end of the list we assume there's only wides left and move one*/
        if (TIMER_STATE.p_next_32 + 2 < p_last_timer) {
            p_next = TIMER_STATE.p_next_32 + 1;
        }
        else if ((TIMER_STATE.p_next_32 + 1)->is_wide
            &&
            (TIMER_STATE.p_next_32 + 2)->is_wide
        ) {
            p_next = TIMER_STATE.p_next_32 + 1;
        }
        else {
            p_next = TIMER_STATE.p_next_32 + 2;
        }

        /* Since 32 bits can be made out of either short or wide timers no need
         * to check for right type. The availability check is a little more
         * complex than in others, so we must check it here. */
        if (p_next <= p_last_timer) {
            /* Increment the counter */
            TIMER_STATE.p_next_32 = p_next;

            /* If the next timer is short we must also check that the next
             * short is available. But this can only happen if next + 1 is in
             * the array */
            if (!p_next->is_wide && (p_next + 1) < p_last_timer) {
                if (p_next->is_available && (p_next + 1)->is_available) {
                    next_timer_found = true;
                }
            }
            /* Or if the next timer is wide, only check if the next is
             * available */
            else if (p_next->is_available) {
                next_timer_found = true;
            }
        }
        /* If we got to the end of the array search all of the array for a
         * possible timer. */
        else {
            for (p_next = p_first_timer; p_next < p_last_timer; ++p_next) {
                /* If a wide timer no need to check the availability of the
                 * other timer (which is NULL) */
                if (p_next->is_wide && p_next->is_available) {
                    TIMER_STATE.p_next_32 = p_next;
                    next_timer_found = true;
                    break;
                }
                /* If the next isn't wide, must also check that the other is
                 * available too. Note that p_next + 1 must lie inside the
                 * array since we're not including p_last_timer in this loop */ 
                else if (
                    p_next->is_available && (p_next + 1)->is_available
                ) {
                    TIMER_STATE.p_next_32 = p_next;
                    next_timer_found = true;
                    break;
                }
            }

            /* Check last timer */
            if (p_last_timer->is_wide && p_last_timer->is_available) {
                TIMER_STATE.p_next_32 = p_last_timer;
                next_timer_found = true;
                break;
            }

            /* If we got through the loop without finding a timer there isn't
             * one left */
            TIMER_STATE.p_next_32 = NULL;
            DEBUG_WRN("All 32 bit timers in use");
        }
    }

    /* 64 bit timer update */
    next_timer_found = false;
    while (TIMER_STATE.p_next_64 != NULL
        &&
        !(
            TIMER_STATE.p_next_64->is_available 
            && 
            (TIMER_STATE.p_next_64 + 1)->is_available
        )
        &&
        !next_timer_found
    ) {
        /* Get next pointer (noting that 64 bit pointers can only increment in
         * 2s, as they require two wide timers). The other is therefore one
         * after the next, since we're always aligned on even timer indexes */
        p_next = TIMER_STATE.p_next_64 + 2;

        /* If next is of the right type increment the pointer (and we're in the
         * array still) */
        if (p_next < p_last_timer && p_next->is_wide) {
            TIMER_STATE.p_next_64 = p_next;
        }
        /* If not the right type, loop from the first timer to the last trying
         * to find one that will work */
        else {
            for (p_next = p_first_timer; p_next < p_last_timer; ++p_next) {
                /* If the right type and available, set the state and set that
                 * the next timer has been found, exit loop. Note that p_next +
                 * 1 must not be NULL as we do not include last in our
                 * iteration. */
                if (p_next->is_wide 
                    && 
                    (p_next->is_available && (p_next + 1)->is_available)
                ) {
                    TIMER_STATE.p_next_64 = p_next;
                    next_timer_found = true;
                    break;
                }
            }

            /* Since 64 bit timers must use two wide timers we don't check the
             * last timer as this would not be possible. */

            /* If we got through the loop without finding a timer there isn't
             * one left */
            TIMER_STATE.p_next_64 = NULL;
            DEBUG_WRN("All 64 bit timers in use");
        }
    }

    return ERROR_NONE;
}

void Timer_disable_specific(Timer_Timer *p_timer_in) {
    /* If the timer is not part of a combined timer */
    if (p_timer_in->p_block->is_split) {
        /* Just disable this timer */
        if (p_timer_in->is_a) {
            TimerDisable(p_timer_in->p_block->base, TIMER_A);
        }
        else {
            TimerDisable(p_timer_in->p_block->base, TIMER_B);
        }

        p_timer_in->is_available = true;
    }
    /* If it's part of a combined timer disable both and split the block */
    else {
        TimerDisable(p_timer_in->p_block->base, TIMER_BOTH);
        p_timer_in->p_block->config = TIMER_CFG_SPLIT_PAIR;
        p_timer_in->p_block->is_split = true;
        
        p_timer_in->p_block->p_a->is_available = true;
        p_timer_in->p_block->p_b->is_available = true;
    }
}