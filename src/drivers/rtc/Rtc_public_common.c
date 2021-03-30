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
    double divisor = (double)RTC_SEC_TO_SUBSEC / (double)RTC_SEC_TO_MILLISEC;
    double ms = (double)(*p_timestamp_in)/divisor;
    return (uint64_t)ms;
}

Rtc_Timestamp Rtc_timestamp_from_ms(
    uint64_t milliseconds_in
) {
    double multiplier = (double)RTC_SEC_TO_SUBSEC/(double)RTC_SEC_TO_MILLISEC;
    double ms = (double)(milliseconds_in)*multiplier;
    return (Rtc_Timestamp)ms;
}

void Rtc_timestamp_to_bytes(
    Rtc_Timestamp *p_timestamp_in,
    uint8_t *p_bytes_out
) {
    uint8_t byte;

    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    for (byte = 0; byte < RTC_TIMESTAMP_NUM_BYTES; ++byte) {
        p_bytes_out[byte] = (uint8_t)(
            (*p_timestamp_in >> ((RTC_TIMESTAMP_NUM_BYTES - 1 - byte) * 8))
            & 0xFF
        );
    }
    #else
    for (byte = 0; byte < RTC_TIMESTAMP_NUM_BYTES; ++byte) {
        p_bytes_out[byte] = (uint8_t)((*p_timestamp_in >> (byte * 8)) & 0xFF);
    }
    #endif
}

Rtc_Timestamp Rtc_timestamp_from_bytes(
    uint8_t *p_bytes_in
) {
    uint8_t byte;
    Rtc_Timestamp timestamp = 0;

    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    for (byte = 0; byte < RTC_TIMESTAMP_NUM_BYTES; ++byte) {
        timestamp = (timestamp << 8) | p_bytes_in[byte];
    }
    #else
    for (byte = 0; byte < RTC_TIMESTAMP_NUM_BYTES; ++byte) {
        timestamp = (timestamp << 8) 
            | p_bytes_in[RTC_TIMESTAMP_NUM_BYTES - 1 - byte];
    }
    #endif

    return timestamp;
}

uint64_t Rtc_timespan_to_ms(
    Rtc_Timespan *p_timespan_in
) {
    double divisor = (double)RTC_SEC_TO_SUBSEC / (double)RTC_SEC_TO_MILLISEC;
    double ms = (double)(*p_timespan_in)/divisor;
    return (uint64_t)ms;
}

Rtc_Timespan Rtc_timespan_from_ms(
    uint64_t milliseconds_in
) {
    double multiplier = (double)RTC_SEC_TO_SUBSEC/(double)RTC_SEC_TO_MILLISEC;
    double ms = (double)(milliseconds_in)*multiplier;
    return (Rtc_Timespan)ms;
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