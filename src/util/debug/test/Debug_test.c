/**
 * @ingroup debug
 * 
 * @file Debug_test.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Test functionality of the Debug module.
 * @version 0.1
 * @date 2020-10-29
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup debug_test Debug Test
 * @{
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

/* -------------------------------------------------------------------------   
 * TESTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Test all log messages
 * 
 * @param state cmocka state.
 */
static void Debug_test_all_log(void **state) {
    (void) state;

    DEBUG_TRC("This is a trace message");
    DEBUG_DBG("This is a debug message");
    DEBUG_INF("This is an info message");
    DEBUG_WRN("This is a warning message");
    DEBUG_ERR("This is an error message");
}

/**
 * @brief Setup function for Debug tests, which inits the Debug module.
 * 
 * @param state cmocka state
 */
static int Debug_test_setup(void **state) {
    if (!Debug_init()) {
        return -1;
    }

    return 0;
}

/* -------------------------------------------------------------------------   
 * TEST GROUP
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests to run for the event manager. 
 */
const struct CMUnitTest debug_tests[] = {
    cmocka_unit_test_setup(
        Debug_test_all_log,
        Debug_test_setup
    )
};

/** @} */ /* end of debug_test */