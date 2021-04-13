/**
 * @file Kernel_test.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Module unit tests for Kernel
 * 
 * Task ref: [UT_2.9.12]
 * 
 * @version 0.1
 * @date 2021-04-13
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
#include "drivers/uart/Uart_errors.h"
#include "components/eps/Eps_errors.h"
#include "applications/power/Power_errors.h"
#include "system/kernel/Kernel_public.h"

/* -------------------------------------------------------------------------   
 * TESTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Test error chain serialisation works correctly
 * 
 * @param state cmocka state
 */
static void Kernel_test_error_serialisation(void **state) {
    (void) state;
    Error uart_error, eps_error, power_error;
    uint8_t buffer[32] = {0};
    uint8_t buffer_length = 0;
    uint8_t expected_buffer[] = {
        (uint8_t)(POWER_ERROR_EPS_SET_OCP_STATE_FAILED & 0xFF),
        (uint8_t)(POWER_ERROR_EPS_SET_OCP_STATE_FAILED >> 8),
        (uint8_t)(EPS_ERROR_INVALID_REQUEST_CRC & 0xFF),
        (uint8_t)(EPS_ERROR_INVALID_REQUEST_CRC >> 8),
        (uint8_t)(UART_ERROR_UNEXPECTED_DEVICE_ID & 0xFF),
        (uint8_t)(UART_ERROR_UNEXPECTED_DEVICE_ID >> 8)
    };
    #ifdef DEBUG_MODE
    char string[64] = {0};
    char expected_string[] = "[D407 8805 6005]";
    #endif

    /* Build a known dummy error chain, which simulates a UART error in EPS */
    /* The uart error is the root cause, it's cause is NULL */
    uart_error.code = UART_ERROR_UNEXPECTED_DEVICE_ID;
    uart_error.p_cause = NULL;

    /* NOTE: currently uart isn't hooked into EPS, so there isn't actually an
     * errorcode for it */
    eps_error.code = EPS_ERROR_INVALID_REQUEST_CRC;
    eps_error.p_cause = &uart_error;

    power_error.code = POWER_ERROR_EPS_SET_OCP_STATE_FAILED;
    power_error.p_cause = &eps_error;

    /* Test serialisation of this error chain */
    Kernel_error_to_bytes(&power_error, buffer, &buffer_length);

    /* Confirm it matches the expected format */
    assert_int_equal(buffer_length, 6);
    assert_memory_equal(
        buffer,
        expected_buffer,
        buffer_length
    );

    /* Confirm the string matches, if we're in debug mode */
    #ifdef DEBUG_MODE
    Kernel_error_to_string(&power_error, string);

    assert_string_equal(
        &string,
        &expected_string
    );
    #endif

    return 0;
}


/* -------------------------------------------------------------------------   
 * TEST GROUP
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests to run for the kernel. 
 */
const struct CMUnitTest kernel_tests[] = {
    cmocka_unit_test(
        Kernel_test_error_serialisation
    )
};