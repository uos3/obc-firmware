/**
 * @file test_all_modules.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Run all module tests.
 * @version 0.1
 * @date 2020-10-29
 * 
 * @copyright Copyright (c) 2020
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
#include "util/debug/test/Debug_test.c"
#include "system/event_manager/test/EventManager_test.c"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {

    int ret = 0;

    /* Debug tests */
    ret |= cmocka_run_group_tests_name(
        "Debug", 
        debug_tests, 
        NULL, NULL
    );

    /* EventManager tests */
    ret |= cmocka_run_group_tests_name(
        "EventManager", 
        eventmanager_tests, 
        NULL, NULL
    );
    
    return ret;
}