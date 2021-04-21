/**
 * @file Eps_test.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Test limited functionality of the Eps module.
 * @version 0.1
 * @date 2021-02-24
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
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "components/eps/Eps_public.h"
#include "components/eps/Eps_private.h"

/* -------------------------------------------------------------------------   
 * TESTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Test that the Eps_do_ocp_states_match function works correctly.
 * 
 * @param state Cmocka state
 * @return int Return code, 0 for success
 */
static int Eps_test_do_ocp_match(void **state) {
    (void) state;
    Eps_OcpState ocp_state_a = {0};
    Eps_OcpState ocp_state_b = {0};

    /* Start with a known ocp_state value, we choose a 010101 pattern for the
     * rails. */
    ocp_state_a.radio_tx = true;
    ocp_state_a.eps_mcu = true;
    ocp_state_a.gnss_rx = true;

    /* Check that the two states aren't equal */
    assert_false(Eps_do_ocp_states_match(
        &ocp_state_a,
        &ocp_state_b
    ));

    /* Now set the two equal and check the return value again */
    ocp_state_b = ocp_state_a;

    assert_true(Eps_do_ocp_states_match(
        &ocp_state_a,
        &ocp_state_b
    ));


    return 0;
}

/**
 * @brief Test the conversion between the different OCP types.
 * 
 * @param state Cmocka state
 * @return int Return code, 0 for success
 */
static int Eps_test_ocp_state_conv(void **state) {
    (void) state;
    Eps_OcpState ocp_state_known = {0};
    Eps_OcpState ocp_state_conv = {0};
    Eps_OcpByte ocp_byte = 0;

    /* Start with a known ocp_state value, we choose a 010101 pattern for the
     * rails. */
    ocp_state_known.radio_tx = true;
    ocp_state_known.eps_mcu = true;
    ocp_state_known.gnss_rx = true;

    /* Confirm that when converted to a byte the value is as expected */
    ocp_byte = Eps_ocp_state_to_ocp_byte(ocp_state_known);
    assert_int_equal((int)ocp_byte, 21);

    /* Convert back from the known byte value and verify that the state is as
     * expected */
    ocp_state_conv = Eps_ocp_byte_to_ocp_state((Eps_OcpByte)21u);

    assert_true(Eps_do_ocp_states_match(
        &ocp_state_known,
        &ocp_state_conv
    ));

    return 0;
}

/**
 * @brief Test the conversion between EPS HK data frame and the Eps_HkData
 * struct. 
 * 
 * @param state Cmocka state
 * @return int Return code, 0 for success
 */
static int Eps_test_parse_hk_data(void **state) {
    (void)state;

    

    return 0;
}

/* -------------------------------------------------------------------------   
 * TEST GROUP
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests to run for the Eps component. 
 */
const struct CMUnitTest eps_tests[] = {
    cmocka_unit_test(
        Eps_test_do_ocp_match
    ),
    cmocka_unit_test(
        Eps_test_ocp_state_conv
    ),
};