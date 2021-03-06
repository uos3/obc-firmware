/**
 * @file Power_test.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Test limited functionality of the Power module.
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
#include "system/opmode_manager/OpModeManager_public.h"
#include "components/eps/Eps_public.h"
#include "applications/power/Power_public.h"
#include "applications/power/Power_private.h"

/* -------------------------------------------------------------------------   
 * TESTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Test the conversion of opmode to ocp rail state 
 * 
 * @param state 
 * @return int 
 */
static int Power_test_get_ocp_state_for_op_mode(void **state) {
    (void) state;
    Eps_OcpState ocp_state;

    /* Get a mode that should be all off */
    ocp_state = Power_get_ocp_state_for_op_mode(
        CFG.POWER_OPMODE_OCP_STATE_CONFIG,
        OPMODEMANAGER_OPMODE_ANTENNA_DEPLOY
    );
    
    /* Check that it's all off except EPS/OBC */
    assert_false(ocp_state.radio_tx);
    assert_false(ocp_state.radio_tx);
    assert_true(ocp_state.eps_mcu);
    assert_true(ocp_state.obc);
    assert_false(ocp_state.gnss_rx);
    assert_false(ocp_state.gnss_lna);

    /* Get a mode that should be all on */
    ocp_state = Power_get_ocp_state_for_op_mode(
        CFG.POWER_OPMODE_OCP_STATE_CONFIG,
        OPMODEMANAGER_OPMODE_NOMINAL_FUNCTIONING
    );
    
    /* Check that it's all on */
    assert_true(ocp_state.radio_tx);
    assert_true(ocp_state.radio_tx);
    assert_true(ocp_state.eps_mcu);
    assert_true(ocp_state.obc);
    assert_true(ocp_state.gnss_rx);
    assert_true(ocp_state.gnss_lna);

    return 0;
}

/**
 * @brief Setup function for Power tests, which inits Power and required 
 * modules.
 * 
 * @param state cmocka state
 */
static int Power_test_setup(void **state) {
    (void) state;

    /* Init DataPool, EventManager, memstoremanager */
    Kernel_init_critical_modules();
    assert_true(Eeprom_init() == ERROR_NONE);
    assert_true(MemStoreManager_init());
    assert_true(OpModeManager_init());
    assert_true(Eps_init());
    
    /* Init the power app */
    assert_true(Power_init());

    return 0;
}

/**
 * @brief Teardown function for Power tests, which just destroys the EM.
 * 
 * @param state cmocka state
 */
static int Power_test_teardown(void **state) {
    (void) state;
    
    if (DP.POWER.ERROR.code != ERROR_NONE) {
        char error_chain[64] = {0};
        Kernel_error_to_string(&DP.POWER.ERROR, &error_chain);
        DEBUG_ERR("DP.POWER.ERROR chain = %s", error_chain);
    }

    EventManager_destroy();

    return 0;
}

/* -------------------------------------------------------------------------   
 * TEST GROUP
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests to run for the Power app. 
 */
const struct CMUnitTest power_tests[] = {
    /* No setup for the init test, which calls init itself */
    cmocka_unit_test_setup_teardown(
        Power_test_get_ocp_state_for_op_mode, 
        Power_test_setup,
        Power_test_teardown
    )
};