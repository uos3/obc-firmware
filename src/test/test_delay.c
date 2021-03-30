/**
 * @file test_delay.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Simple test of the delay module.
 * 
 * The delay module isn't supposed to be accurate, but it is supposed to
 * provide simple delay blocking delay capabilities. To test this we will run
 * it against the RTC module, recording the RTC value before and after a delay,
 * and confirming they are within 2-sigma (95%, arbitraray requirement on
 * accuracy that I (drh) belive to be adequate). 
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
#include <math.h>
#include <stdlib.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/rtc/Rtc_public.h"
#include "drivers/delay/Delay_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

int main(void) {
    ErrorCode error;
    Rtc_Timestamp start;
    Rtc_Timestamp end;
    /* Use 10 s delay */
    uint32_t expected_delay_ms = 10000;
    Rtc_Timespan actual_delay;
    uint64_t actual_delay_ms;
    /* Minimum accuracy of 2 sigma (95% accuracy) */
    double min_accuracy = 0.05;
    double accuracy;

    /* Init system critical */
    Kernel_init_critical_modules();

    DEBUG_INF("---- DELAY TEST ----");
    DEBUG_INF("Initialising Rtc...");

    /* Init RTC */
    error = Rtc_init();
    if (error != ERROR_NONE) {
        Debug_exit(1);
    }

    DEBUG_INF("Rtc init complete, starting 10 s delay to compare with RTC");

    /* Record start time */
    start = Rtc_get_timestamp();

    /* Delay for required time */
    Delay_ms(expected_delay_ms);

    /* Record the end timestamp */
    end = Rtc_get_timestamp();

    /* Get the difference between them, and compare it to the expected number
     * of milliseconds delay */
    actual_delay = end - start;
    actual_delay_ms = Rtc_timespan_to_ms(&actual_delay);
    accuracy = (double)((uint64_t)actual_delay_ms - expected_delay_ms)
        /
        (double)expected_delay_ms;

    DEBUG_INF(
        "Delay complete, expected %d ms delay, recorded %ld ms delay",
        expected_delay_ms,
        actual_delay_ms  
    );

    /* Check the accuracy */
    if (fabs(accuracy) <= min_accuracy) {
        DEBUG_INF(
            "Delay was accurate to RTC with a %.4f error, required accuracy is +-%.4f", 
            accuracy,
            min_accuracy
        );
        DEBUG_INF("Test passed");
        return EXIT_SUCCESS;
    }
    else {
        DEBUG_ERR(
            "Delay was inaccurate to RTC with a %.4f error, required accuracy is +-%.4f", 
            accuracy,
            min_accuracy
        );
        DEBUG_ERR("Test failed");
        return EXIT_FAILURE;
    }
}