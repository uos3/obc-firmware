/**
 * @file Rtc_test.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Tests for common implementations of RTC systems.
 * 
 * Task ref: [UT_2.8.4]
 * 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

/* External library includes */
#include <cmocka.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/rtc/Rtc_public.h"

/* -------------------------------------------------------------------------   
 * TESTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Test conversion of Timestamp/Timespan to/from milliseconds
 * 
 * @param state cmocka state
 */
void Rtc_test_to_from_ms(void **state) {
    (void) state;

    Rtc_Timestamp timestamp;
    Rtc_Timespan timespan;

    /* ---- TIMESTAMP ---- */

    /* min value */
    timestamp = 0;
    assert_int_equal(Rtc_timestamp_to_ms(&timestamp), 0);

    /* max value (2^48 - 1) */
    timestamp = 281474976710655;
    assert_int_equal(Rtc_timestamp_to_ms(&timestamp), 8589934591999);

    /* min value */
    timestamp = Rtc_timestamp_from_ms(0);
    assert_int_equal(timestamp, 0);

    /* max value (2^48 - 1) */
    timestamp = Rtc_timestamp_from_ms(8589934591999);
    assert_int_equal(timestamp, 281474976710623);
    
    /* ---- TIMESPAN ---- */

    /* min value */
    timespan = 0;
    assert_int_equal(Rtc_timespan_to_ms(&timespan), 0);

    /* max value (2^48 - 1) */
    timespan = 281474976710655;
    assert_int_equal(Rtc_timespan_to_ms(&timespan), 8589934591999);

    /* min value */
    timespan = Rtc_timespan_from_ms(0);
    assert_int_equal(timespan, 0);

    /* max value (2^48 - 1) */
    timespan = Rtc_timespan_from_ms(8589934591999);
    assert_int_equal(timespan, 281474976710623);
}

/**
 * @brief Test conversions of timestamps to/from bytes
 * 
 * @param state cmocka state
 */
void Rtc_test_to_from_bytes(void **state) {
    (void) state;

    Rtc_Timestamp timestamp = 0x010203040506;
    uint8_t bytes[RTC_TIMESTAMP_NUM_BYTES] = {0};
    uint8_t expected_bytes[RTC_TIMESTAMP_NUM_BYTES] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06
    };

    /* Test conversion to bytes, using obvious pattern */
    Rtc_timestamp_to_bytes(&timestamp, &bytes);
    assert_memory_equal(&bytes, &expected_bytes, RTC_TIMESTAMP_NUM_BYTES);

    /* Test conversion from expected bytes to timestamp */
    timestamp = Rtc_timestamp_from_bytes(&bytes);
    assert_int_equal(timestamp, 0x010203040506);
}

/* -------------------------------------------------------------------------   
 * TEST GROUP
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests to run for the Rtc module. 
 */
const struct CMUnitTest rtc_tests[] = {
    cmocka_unit_test(
        Rtc_test_to_from_ms
    ),
    cmocka_unit_test(
        Rtc_test_to_from_bytes
    )
};