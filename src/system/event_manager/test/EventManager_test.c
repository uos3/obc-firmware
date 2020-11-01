/**
 * @ingroup event_manager
 * 
 * @file EventManager_test.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Test functionality of the EventManager module.
 * @version 0.1
 * @date 2020-10-29
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup event_manager_test Event Manager Test
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
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * TESTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Test that the init function behaves as expected
 * 
 * @param state cmocka state
 */
static void EventManager_test_init(void **state) {
    (void) state;

    /* Check that the init function succeeds */
    assert_true(EventManager_init());

    /* Check init flag is set */
    assert_true(EVENTMANAGER.initialised);
    
    /* Check that all pointers in the EventManager static are non-null */
    assert_non_null(EVENTMANAGER.p_raised_events);
    assert_non_null(EVENTMANAGER.p_num_cycles_events_raised);

    /* Check that the list size is the minimum size */
    assert_int_equal(EVENTMANAGER.size_of_lists, EVENTMANAGER_MIN_LIST_SIZE);
    
    /* Check there are no events */
    assert_int_equal(EVENTMANAGER.num_raised_events, 0);
}

/**
 * @brief Test that _raise_event works correctly.
 * 
 * @param state cmocka state.
 */
static void EventManager_test_raise_event(void **state) {
    (void) state;

    /* Raise 64 events, which will require expanding the event list */
    for (int i = 1; i <= 64; i++) {
        assert_true(EventManager_raise_event((Event)i));
    }

    /* Check size and num of events */
    assert_int_equal(EVENTMANAGER.num_raised_events, 64);
    assert_int_equal(EVENTMANAGER.size_of_lists, 128);

    /* Check all events are in the list */
    for (int i = 0; i < 64; i++) {
        assert_int_equal(EVENTMANAGER.p_raised_events[i], i + 1);
    }
}

/**
 * @brief Test that _is_event_raised works correctly.
 * 
 * @param state 
 */
static void EventManager_test_is_event_raised(void **state) {
    (void) state;

    /* Check that no event is raised at the start */
    bool is_raised = true;
    assert_true(EventManager_is_event_raised((Event)1, &is_raised));
    assert_false(is_raised);

    /* Raise an event and check it is raised */
    assert_true(EventManager_raise_event((Event)1));
    assert_true(EventManager_is_event_raised((Event)1, &is_raised));
    assert_true(is_raised);
}

/**
 * @brief Test that _poll_event works correctly.
 * 
 * @param state cmocka state
 */
static void EventManager_test_poll_event(void **state) {
    (void) state;

    /* Raise an event */
    assert_true(EventManager_raise_event((Event)2));

    /* Poll that event and check that the raised flag is true */
    bool is_raised = false;
    assert_true(EventManager_poll_event((Event)2, &is_raised));
    assert_true(is_raised);

    /* Check that the event has been removed */
    assert_int_equal(EVENTMANAGER.num_raised_events, 0);

    /* Poll an event that isn't raised and check the raised flag is false. */
    assert_true(EventManager_poll_event((Event)1, &is_raised));
    assert_false(is_raised);

    /* Check that there are no events */
    assert_int_equal(EVENTMANAGER.num_raised_events, 0);
}

/**
 * @brief Test that _cleanup_events works correctly.
 * 
 * @param state cmocka state
 */
static void EventManager_test_cleanup_events(void **state) {
    (void) state;

    /* Raise 64 events */
    for (int i = 1; i <= 64; i++) {
        assert_true(EventManager_raise_event((Event)i));
    }

    /* Check all events are present */
    assert_int_equal(EVENTMANAGER.num_raised_events, 64);

    /* Check all cycle counters are zero */
    for (int i = 0; i < 64; i++) {
        assert_int_equal(EVENTMANAGER.p_num_cycles_events_raised[i], 0);
    }

    /* Call cleanup */
    assert_true(EventManager_cleanup_events());

    /* Check there are still 64 events */
    assert_int_equal(EVENTMANAGER.num_raised_events, 64);

    /* Check all counters are one */
    for (int i = 0; i < 64; i++) {
        assert_int_equal(EVENTMANAGER.p_num_cycles_events_raised[i], 1);
    }

    /* Call cleanup */
    assert_true(EventManager_cleanup_events());

    /* Check there are no events */
    assert_int_equal(EVENTMANAGER.num_raised_events, 0);
}

/**
 * @brief Setup function for EventManager tests, which inits EM.
 * 
 * @param state cmocka state
 */
static int EventManager_test_setup(void **state) {
    if (!EventManager_init()) {
        return -1;
    }

    return 0;
}

/**
 * @brief Teardown function for EventManager tests, which just destroys the EM.
 * 
 * @param state cmocka state
 */
static int EventManager_test_teardown(void **state) {
    EventManager_destroy();

    return 0;
}

/* -------------------------------------------------------------------------   
 * TEST GROUP
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests to run for the event manager. 
 */
const struct CMUnitTest eventmanager_tests[] = {
    /* No setup for the init test, which calls init itself */
    cmocka_unit_test_teardown(
        EventManager_test_init, 
        EventManager_test_teardown
    ),
    cmocka_unit_test_setup_teardown(
        EventManager_test_raise_event,
        EventManager_test_setup,
        EventManager_test_teardown
    ),
    cmocka_unit_test_setup_teardown(
        EventManager_test_is_event_raised,
        EventManager_test_setup,
        EventManager_test_teardown
    ),
    cmocka_unit_test_setup_teardown(
        EventManager_test_poll_event,
        EventManager_test_setup,
        EventManager_test_teardown
    ),
    cmocka_unit_test_setup_teardown(
        EventManager_test_cleanup_events,
        EventManager_test_setup,
        EventManager_test_teardown
    ),
};

/** @} */ /* End of event_manager_test */