/**
 * @file Rtc_public_common.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Common target Rtc module public implementation.
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

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/rtc/Rtc_public.h"
#include "drivers/rtc/Rtc_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

uint64_t Rtc_timestamp_to_ms(
    Rtc_Timestamp *p_timestamp_in
) {
    return (*p_timestamp_in / (RTC_SEC_TO_SUBSEC / RTC_SEC_TO_MILLISEC));
}

Rtc_Timestamp Rtc_timestamp_from_ms(
    uint64_t milliseconds_in
) {
    return (milliseconds_in * (RTC_SEC_TO_SUBSEC / RTC_SEC_TO_MILLISEC));
}

uint64_t Rtc_timespan_to_ms(
    Rtc_Timespan *p_timespan_in
) {
    return (*p_timespan_in / (RTC_SEC_TO_SUBSEC / RTC_SEC_TO_MILLISEC));
}

Rtc_Timespan Rtc_timespan_from_ms(
    uint64_t milliseconds_in
) {
    return (milliseconds_in * (RTC_SEC_TO_SUBSEC / RTC_SEC_TO_MILLISEC));
}

ErrorCode Rtc_is_timespan_ellapsed(
    Rtc_Timestamp *p_start_in, 
    Rtc_Timespan *p_duration_in, 
    bool *p_is_ellapsed_out
) {
    Rtc_Timestamp current;
    Rtc_Timestamp end;

    /* Get the current time */
    current = Rtc_get_timestamp();

    /* If the current time is before the start return an error */
    if (current < *p_start_in) {
        return RTC_ERROR_TIME_IN_PAST;
    }

    /* Calculate the end time */
    end = *p_start_in + *p_duration_in;

    /* If end is in the future the duration hasn't ellapsed */
    if (end >= current) {
        *p_is_ellapsed_out = false;
    }
    /* Otherwise the duration has elapsed */
    else {
        *p_is_ellapsed_out = true;
    }

    return ERROR_NONE;
}