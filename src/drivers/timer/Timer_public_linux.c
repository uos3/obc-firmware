/**
 * @file Timer_public_linux.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Implements the timer driver for the software on the Linux platform.
 * 
 * Task ref: [UT_2.8.1]
 * 
 * The Linux timer implementation is inspired by the one found at 
 * https://stackoverflow.com/questions/12764771/timers-in-linux-in-c/12765121.
 * This driver is not meant to emulate the physical behaviour of the TM4C,
 * instead it is supposed to provide a functioning interface for other code to
 * use to be tested on Linux.
 * 
 * This is to allow high level code to be developed and tested with a reliable
 * timer API.
 * 
 * @version 0.1
 * @date 2021-02-04
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* System includes */
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/timer/Timer_public.h"
#include "drivers/timer/Timer_private.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Signal to use for timers
 */
#define TIMER_SIGNAL (SIGRTMIN+0)

/**
 * @brief Bitflag indicating the timer is in use
 */
#define TIMER_USED (1)

/**
 * @brief Bitflag indicating the timer has been armed
 */
#define TIMER_ARMED (2)

/**
 * @brief Bitflag indicating the timer should be periodic
 */
#define TIMER_PERIODIC (4)

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief Timer instance used to trigger all timers
 */
static timer_t TIMER_TIMER;

/**
 * @brief Atomically modifiable timer states.
 */
static volatile sig_atomic_t TIMER_LINUX_STATE[TIMER_NUM_TIMERS] = { 0 };

/**
 * @brief Times at which each timer should expire.
 */
static struct itimerspec TIMER_TIME[TIMER_NUM_TIMERS];

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/* Return the number of seconds between before and after, (after - before).
 * This must be async-signal safe, so it cannot use difftime().
*/
static inline double timespec_diff(
    const struct timespec after, 
    const struct timespec before
) {
    return (double)(after.tv_sec - before.tv_sec)
         + (double)(after.tv_nsec - before.tv_nsec) / 1000000000.0;
}

/* Add positive seconds to a timespec, nothing if seconds is negative.
 * This must be async-signal safe. */
static inline void timespec_add_seconds(
    struct timespec *const to, 
    const double seconds
) {
    if (to && seconds > 0.0) {
        long  s = (long)seconds;
        long  ns = (long)(0.5 + 1000000000.0 * (seconds - (double)s));

        /* Adjust for rounding errors. */
        if (ns < 0L)
            ns = 0L;
        else
        if (ns > 999999999L)
            ns = 999999999L;

        to->tv_sec += (time_t)s;
        to->tv_nsec += ns;

        if (to->tv_nsec >= 1000000000L) {
            to->tv_nsec -= 1000000000L;
            to->tv_sec++;
        }
    }
}

static inline struct timespec timespec_from_seconds(
    const double seconds
) {
    long s = (long)seconds;
    long ns = (long)(0.5 + 1000000000.0 * (seconds - (double)s));

    struct timespec out;
    out.tv_sec = s;
    out.tv_nsec = ns;

    return out;
}

/* Set the timespec to the specified number of seconds, or zero if negative 
 * seconds. */
static inline void timespec_set_seconds(
    struct timespec *const to, 
    const double seconds
) {
    if (to) {
        if (seconds > 0.0) {
            const long  s = (long)seconds;
            long       ns = (long)(0.5 + 1000000000.0 * (seconds - (double)s));

            if (ns < 0L)
                ns = 0L;
            else
            if (ns > 999999999L)
                ns = 999999999L;

            to->tv_sec = (time_t)s;
            to->tv_nsec = ns;

        } else {
            to->tv_sec = (time_t)0;
            to->tv_nsec = 0L;
        }
    }
}

/* Add two timespects together and store the result in `to` */
static inline void timespec_add(
    struct timespec *const to,
    const struct timespec from
) {
    to->tv_nsec += from.tv_nsec;
    to->tv_sec += from.tv_sec;

    if (to->tv_nsec >= 1000000000L) {
        to->tv_nsec -= 1000000000L;
        to->tv_sec++;
    }
}


/**
 * @brief Signal handler for the timer.
 * 
 * Strictly speaking this should be in a private_linux.c file, but there's no
 * real reason for that for this little hacky implementation.
 */
void Timer_signal_handler(
    int signum __attribute__((unused)), 
    siginfo_t *p_info, 
    void *p_context __attribute__((unused))
) {
    struct timespec now;
    struct itimerspec when;
    int saved_errno, i;
    double next;

    /* If not a timer signal exit now. */
    if (!p_info || p_info->si_code != SI_TIMER) {
        return;
    }

    /* Save errno, some of the functions may modify it */
    saved_errno = errno;

    /* Get the current time */
    if (clock_gettime(CLOCK_REALTIME, &now)) {
        errno = saved_errno;
        return;
    }

    /* Assume there's no timer next */
    next = -1.0;

    /* Check all timers that are used and armed, but haven't passed yet. */
    for (i = 0; i < TIMER_NUM_TIMERS; ++i) {
        if ((__sync_or_and_fetch(&TIMER_LINUX_STATE[i], 0) 
            & 
            (TIMER_USED | TIMER_ARMED))
        ) {
            /* Get the number of seconds between now and the timer */
            double seconds = timespec_diff(TIMER_TIME[i].it_value, now);

            /* If passed, fire the timer's event. We count on the events being
             * mapped linearly with timer index */
            if (seconds <= 0.0) {
                if (!EventManager_raise_event(EVT_TIMER_00A_COMPLETE + i)) {
                    DEBUG_ERR("Failed to raise event in Timer signal handler");
                    return;
                }
                /* If periodic increment the time the time */
                if (__sync_or_and_fetch(&TIMER_LINUX_STATE[i], 0) 
                    & 
                    TIMER_PERIODIC
                ) {
                    timespec_add(
                        &TIMER_TIME[i].it_value, 
                        TIMER_TIME[i].it_interval
                    );

                    /* reset the seconds counter for this one */
                    seconds = timespec_diff(TIMER_TIME[i].it_value, now);
                }
            }
            
            if (next <= 0.0 || (seconds >= 0.0 && seconds < next)) {
                /* This is the soonest timer in the future */
                next = seconds;
            }
        }
    }

    /* Note: timespec_set() will set the time to zero if next <= 0.0,
     *       which in turn will disarm the timer.
     * The timer is one-shot; it_interval == 0.
    */
    timespec_set_seconds(&when.it_value, next);
    when.it_interval.tv_sec = 0;
    when.it_interval.tv_nsec = 0L;
    timer_settime(TIMER_TIMER, 0, &when, NULL);

    errno = saved_errno;
}

ErrorCode Timer_set(
    const double seconds_in, 
    bool periodic_in,
    Event *p_event_out
) {
    struct timespec   now, then;
    struct itimerspec when;
    double            next;
    int               timer, i, state;

    /* Timeout must be in the future. */
    if (seconds_in <= 0.0) {
        DEBUG_ERR("Negative timer duration not allowed");
        return TIMER_ERROR_DURATION_NEGATIVE;
    }

    /* Get current time, */
    if (clock_gettime(CLOCK_REALTIME, &now)) {
        DEBUG_ERR("Couldn't get current clock time");
        return TIMER_ERROR_NULL_TIMER;
    }

    /* and calculate when the timer should fire. */
    then = now;
    timespec_add_seconds(&then, seconds_in);

    /* Find an unused timer. */
    for (timer = 0; timer < TIMER_NUM_TIMERS; timer++)
        if (!(__sync_fetch_and_or(&TIMER_LINUX_STATE[timer], TIMER_USED) 
            & TIMER_USED
        )) {
            break;
        }

    /* No unused timeouts? */
    if (timer >= TIMER_NUM_TIMERS) {
        DEBUG_ERR("All timers in use, cannot create a new one");
        return TIMER_ERROR_NO_64_BIT_TIMERS;
    }

    /* Clear all but TIMEOUT_USED and set the periodicty */
    if (periodic_in) {
        state = TIMER_USED | TIMER_PERIODIC;
    }
    else {
        state = TIMER_USED;
    }
    __sync_and_and_fetch(&TIMER_LINUX_STATE[timer], state);

    /* update the timer details, */
    TIMER_TIME[timer].it_value = then;

    if (periodic_in) {
        TIMER_TIME[timer].it_interval = timespec_from_seconds(seconds_in);
    }

    /* and mark the timer armable. */
    __sync_or_and_fetch(&TIMER_LINUX_STATE[timer], TIMER_ARMED);

    /* How long till the next timeout? */
    next = seconds_in;
    for (i = 0; i < TIMER_NUM_TIMERS; i++) {
        if ((__sync_fetch_and_or(&TIMER_LINUX_STATE[i], 0) 
            & 
            (TIMER_USED | TIMER_ARMED)) 
            == 
            (TIMER_USED | TIMER_ARMED)
        ) {
            const double secs = timespec_diff(TIMER_TIME[i].it_value, now);
            if (secs >= 0.0 && secs < next)
                next = secs;
        }
    }

    /* Calculate duration when to fire the timeout next, */
    timespec_set_seconds(&when.it_value, next);
    when.it_interval.tv_sec = 0;
    when.it_interval.tv_nsec = 0L;

    /* and arm the timer. */
    if (timer_settime(TIMER_TIMER, 0, &when, NULL)) {
        /* Failed. */
        __sync_and_and_fetch(&TIMER_LINUX_STATE[timer], 0);
        DEBUG_ERR("Couldn't set timer");
        return TIMER_ERROR_NO_FACTORS_FOUND;
    }

    /* Set the event number */
    *p_event_out = (Event)(EVT_TIMER_00A_COMPLETE + (Event)timer);

    return ERROR_NONE;
}

ErrorCode Timer_init(void) {
    
    struct sigaction sig_act;
    struct sigevent sig_evt;
    struct itimerspec timer_spec;

    /* Build signal for the timers */
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_sigaction = Timer_signal_handler;
    sig_act.sa_flags = SA_SIGINFO;

    /* Install the timer signal handler */
    if (sigaction(TIMER_SIGNAL, &sig_act, NULL)) {
        DEBUG_ERR("Couldn't set signal handler for Timer");
        /* Not technically the right error but don't want to add new error
         * definitions, and this one is *kind of* similar in contex. */
        return TIMER_ERROR_PERIPH_ENABLE_FAILED;
    }

    /* Create timer to send the signal */
    sig_evt.sigev_notify = SIGEV_SIGNAL;
    sig_evt.sigev_signo = TIMER_SIGNAL;
    sig_evt.sigev_value.sival_ptr = NULL;
    if (timer_create(CLOCK_REALTIME, &sig_evt, &TIMER_TIMER)) {
        DEBUG_ERR("Couldn't create Timer");
        /* Not technically the right error but don't want to add new error
         * definitions, and this one is *kind of* similar in contex. */
        return TIMER_ERROR_PERIPH_ENABLE_FAILED;
    }

    /* Disable the timer for now */
    timer_spec.it_value.tv_sec = 0;
    timer_spec.it_value.tv_nsec = 0L;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0L;
    if (timer_settime(TIMER_TIMER, 0, &timer_spec, NULL)) {
        DEBUG_ERR("Couldn't initialise Timer to zero");
        /* Not technically the right error but don't want to add new error
         * definitions, and this one is *kind of* similar in contex. */
        return TIMER_ERROR_PERIPH_ENABLE_FAILED;
    }

    return ERROR_NONE;
}

ErrorCode Timer_start_one_shot(
    double duration_s_in, 
    Event *p_timer_event_out
) {
    return Timer_set(duration_s_in, false, p_timer_event_out);
}

ErrorCode Timer_start_periodic(
    double duration_s_in, 
    Event *p_timer_event_out
) {
    return Timer_set(duration_s_in, true, p_timer_event_out);
}