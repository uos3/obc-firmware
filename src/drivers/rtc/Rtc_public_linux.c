/**
 * @file Rtc_public_tm4c.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Linux target Rtc module public implementation.
 * 
 * Task ref: [UT_2.8.4]
 * 
 * @version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "drivers/delay/Delay_public.h"
#include "drivers/rtc/Rtc_public.h"
#include "drivers/rtc/Rtc_private.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

struct timespec RTC_EPOCH_TIMESPEC;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Rtc_init(void) {
    int res;

    /* All we have to do is get the current time and set it in the epoch. */
    res = clock_gettime(CLOCK_REALTIME, &RTC_EPOCH_TIMESPEC);

    /* Check for error */
    if (res < 0) {
        DEBUG_ERR("Couldn't init RTC (linux), errno = %d", errno);
        return RTC_ERROR_PERIPHERAL_NOT_INIT;
    }

    /* We include a delay here to emulate the behaviour of the TM4C driver,
     * which has to wait for the oscillator to stabilise */
    Delay_ms(RTC_OSCILLATOR_STABILISE_DELAY_MS);

    /* Set the initialised flag */
    DP.RTC_INITIALISED = true;

    return ERROR_NONE;
}

Rtc_Timestamp Rtc_get_timestamp(void) {
    int res;
    struct timespec current;
    uint64_t nanosecs;

    /* All we have to do is get the current time and set it in the epoch. */
    res = clock_gettime(CLOCK_REALTIME, &current);

    /* Check for error */
    if (res < 0) {
        DEBUG_ERR("Couldn't get RTC (linux) time, errno = %d", errno);
        /* no way to return error, exit immediately */
        Debug_exit(1);
    }

    /* Find the difference between the epoch and the current time in
     * nanoseconds */
    nanosecs = (uint64_t)(
        (current.tv_sec - RTC_EPOCH_TIMESPEC.tv_sec)*1000000000
        +
        (current.tv_nsec - RTC_EPOCH_TIMESPEC.tv_sec)
    );

    /* Convert the nanoseconds into a number of subseconds */
    return nanosecs / (1000000000 / RTC_SEC_TO_SUBSEC);
}

void Rtc_set_seconds(uint32_t seconds_in) {
    RTC_EPOCH_TIMESPEC.tv_sec = RTC_EPOCH_TIMESPEC.tv_sec + seconds_in;
}