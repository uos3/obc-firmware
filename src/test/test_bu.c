/**
 * @file test_bu.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Tests BU mode of the OpModeManager.
 * 
 * Task ref: [UT_2.9.10]
 * 
 * @version 0.1
 * @date 2021-04-01
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdbool.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/opmode_manager/OpModeManager_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "drivers/rtc/Rtc_public.h"
#include "drivers/timer/Timer_public.h"
#include "drivers/udma/Udma_public.h"
#include "drivers/uart/Uart_public.h"
#include "applications/power/Power_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Timeout on any action
 */
#define ACTIVITY_TIMEOUT_MS (10000)

/* -------------------------------------------------------------------------   
 * STATICS
 * ------------------------------------------------------------------------- */

static MemStoreManager_PersistentData ORIG_PERS_DATA;

/* -------------------------------------------------------------------------   
 * FUNCTION DECLARATIONS
 * ------------------------------------------------------------------------- */

void fail_test(void);

bool cyclic_processing(void);

void clean_between_tests();

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
    ErrorCode error;
    Rtc_Timestamp activity_start;
    bool is_activity_timeout = false;
    bool run_loop = true;
    bool cyclic_success;
    MemStoreManager_PersistentData temp_pers_data;
    Rtc_Timespan activity_timeout_timespan = Rtc_timespan_from_ms(
        ACTIVITY_TIMEOUT_MS
    );

    /* Kernel early init */
    Kernel_init_critical_modules();
    
    DEBUG_INF("---- BU TEST ----");

    DEBUG_INF("Init Rtc...");
    error = Rtc_init();
    if (error != ERROR_NONE) {
        Debug_exit(1);
    }

    DEBUG_INF("Init Eeprom...");
    error = Eeprom_init();
    if (error != ERROR_NONE) {
        Debug_exit(1);
    }

    DEBUG_INF("Init MemStoreManager...");
    if (!MemStoreManager_init()) {
        Debug_exit(1);
    }

    DEBUG_INF("Init Udma...");
    if (!Udma_init()) {
        Debug_exit(1);
    }

    DEBUG_INF("Init Uart...");
    if (!Uart_init()) {
        Debug_exit(1);
    }

    DEBUG_INF("Init Eps...");
    if (!Eps_init()) {
        Debug_exit(1);
    }

    DEBUG_INF("Init Power...");
    if (!Power_init()) {
        Debug_exit(1);
    }

    /* Get a copy of the pers data to reset at the end of the test */
    ORIG_PERS_DATA = MemStoreManager_get_pers_data();
    temp_pers_data = ORIG_PERS_DATA;

    /* First test: standard init from cold, AD not complete, Timer not
     * initialised. */
    DEBUG_INF("---- TEST 1: COLD BOOT, TIMER NOT INIT ----");
    temp_pers_data.last_opmode = 0;
    temp_pers_data.antenna_deployed = false;
    temp_pers_data.num_antenna_deploy_attempts = 0;
    MemStoreManager_set_pers_data(temp_pers_data);

    activity_start = Rtc_get_timestamp();
    DEBUG_INF("Init OpModeManager, expect dwell time test");
    if (!OpModeManager_init()) {
        DEBUG_ERR("OpModeManager_init failed");
        fail_test();
    }

    if (!DP.OPMODEMANAGER.BU_DWELL_CHECK_RTC) {
        DEBUG_ERR(
            "Timer not initialised but BU still trying to use Timer"
        );
        fail_test();
    }
    if (DP.OPMODEMANAGER.BU_DWELL_TIMER_EVENT != EVT_NONE) {
        DEBUG_ERR(
            "Timer event set in BU mode when RTC should be used"
        );
        fail_test();
    }

    /* Start cyclic processing for this activity */
    while (run_loop) {
        cyclic_success = cyclic_processing();
        run_loop &= cyclic_success;

        Rtc_is_timespan_ellapsed(
            &activity_start,
            &activity_timeout_timespan,
            &is_activity_timeout
        );
        run_loop &= !is_activity_timeout;

        /* Check exit condition, OPMODE != BU */
        if (DP.OPMODEMANAGER.OPMODE != OPMODEMANAGER_OPMODE_BOOT_UP) {
            run_loop = false;
        }

        /* Do event cleanup */
        EventManager_cleanup_events();
    }

    if (is_activity_timeout) {
        DEBUG_ERR("Switch to AD timedout");
        fail_test();
    }

    if (!cyclic_success) {
        DEBUG_ERR("Error running cyclic processing");
        fail_test();
    }

    DEBUG_INF("---- TEST 1 PASSED ----");

    clean_between_tests();
    /* Also reset the RTC to check that the dwell time is tested again */
    Rtc_set_seconds(0);

    /* Initialise Timer to test with that */
    DEBUG_INF("Init Timer...");
    error = Timer_init();
    if (error != ERROR_NONE) {
        DEBUG_ERR("Timer_init failed");
        fail_test();
    }

    /* Second test: standard init from cold, AD not complete, Timer properly
     * initialised. */
    DEBUG_INF("---- TEST 2: COLD BOOT, TIMER INIT ----");
    temp_pers_data.last_opmode = 0;
    temp_pers_data.antenna_deployed = false;
    temp_pers_data.num_antenna_deploy_attempts = 0;
    MemStoreManager_set_pers_data(temp_pers_data);

    activity_start = Rtc_get_timestamp();
    DEBUG_INF("Init OpModeManager, expect dwell time test");
    if (!OpModeManager_init()) {
        DEBUG_ERR("OpModeManager_init failed");
        fail_test();
    }

    if (DP.OPMODEMANAGER.BU_DWELL_CHECK_RTC) {
        DEBUG_ERR(
            "Timer initialised but BU not using RTC"
        );
        fail_test();
    }
    if (DP.OPMODEMANAGER.BU_DWELL_TIMER_EVENT == EVT_NONE) {
        DEBUG_ERR(
            "Timer event not set in BU mode when it should be"
        );
        fail_test();
    }

    /* Start cyclic processing for this activity */
    run_loop = true;
    while (run_loop) {
        cyclic_success = cyclic_processing();
        run_loop &= cyclic_success;

        Rtc_is_timespan_ellapsed(
            &activity_start,
            &activity_timeout_timespan,
            &is_activity_timeout
        );
        run_loop &= !is_activity_timeout;

        /* Check exit condition, OPMODE != BU */
        if (DP.OPMODEMANAGER.OPMODE != OPMODEMANAGER_OPMODE_BOOT_UP) {
            run_loop = false;
        }
    }

    if (is_activity_timeout) {
        DEBUG_ERR("Switch to AD timedout");
        fail_test();
    }

    if (!cyclic_success) {
        DEBUG_ERR("Error running cyclic processing");
        fail_test();
    }

    DEBUG_INF("---- TEST 2 PASSED ----");

    /* Clean for next test */
    clean_between_tests();

    /* Third test: AD complete */
    DEBUG_INF("---- TEST 3: BOOT AFTER AD ----");
    temp_pers_data.last_opmode = 0;
    temp_pers_data.antenna_deployed = true;
    temp_pers_data.num_antenna_deploy_attempts = 1;
    MemStoreManager_set_pers_data(temp_pers_data);

    activity_start = Rtc_get_timestamp();
    DEBUG_INF("Init OpModeManager, expect to check LP first then to NF");
    if (!OpModeManager_init()) {
        DEBUG_ERR("OpModeManager_init failed");
        fail_test();
    }

    if (DP.OPMODEMANAGER.BU_DWELL_CHECK_RTC) {
        DEBUG_ERR("RTC being checked after dwell time has already passed");
        fail_test();
    }
    if (DP.OPMODEMANAGER.BU_DWELL_TIMER_EVENT != EVT_NONE) {
        DEBUG_ERR("Timer event set in BU mode when it shouldn't be");
        fail_test();
    }
    /* Start cyclic processing for this activity */
    run_loop = true;
    while (run_loop) {
        cyclic_success = cyclic_processing();
        run_loop &= cyclic_success;

        Rtc_is_timespan_ellapsed(
            &activity_start,
            &activity_timeout_timespan,
            &is_activity_timeout
        );
        run_loop &= !is_activity_timeout;

        /* Check exit condition, OPMODE != BU */
        if (DP.OPMODEMANAGER.OPMODE != OPMODEMANAGER_OPMODE_BOOT_UP) {
            run_loop = false;
        }
    }

    if (is_activity_timeout) {
        DEBUG_ERR("Switch to NF timedout");
        fail_test();
    }

    if (!cyclic_success) {
        DEBUG_ERR("Error running cyclic processing");
        fail_test();
    }

    DEBUG_INF("---- TEST 3 PASSED ----");

    /* Clean for next test */
    clean_between_tests();

    /* Third test: AD complete */
    DEBUG_INF("---- TEST 4: BOOT AFTER SM ----");
    temp_pers_data.last_opmode = OPMODEMANAGER_OPMODE_SAFE;
    temp_pers_data.antenna_deployed = true;
    temp_pers_data.num_antenna_deploy_attempts = 1;
    MemStoreManager_set_pers_data(temp_pers_data);

    activity_start = Rtc_get_timestamp();
    DEBUG_INF("Init OpModeManager, expect straight boot to SM");
    if (!OpModeManager_init()) {
        DEBUG_ERR("OpModeManager_init failed");
        fail_test();
    }

    if (DP.OPMODEMANAGER.NEXT_OPMODE != OPMODEMANAGER_OPMODE_SAFE) {
        DEBUG_ERR("Expected next OPMODE to be SAFE");
        fail_test();
    }

    /* Start cyclic processing for this activity */
    run_loop = true;
    while (run_loop) {
        cyclic_success = cyclic_processing();
        run_loop &= cyclic_success;

        Rtc_is_timespan_ellapsed(
            &activity_start,
            &activity_timeout_timespan,
            &is_activity_timeout
        );
        run_loop &= !is_activity_timeout;

        /* Check exit condition, OPMODE != BU */
        if (DP.OPMODEMANAGER.OPMODE != OPMODEMANAGER_OPMODE_BOOT_UP) {
            run_loop = false;
        }
    }

    if (is_activity_timeout) {
        DEBUG_ERR("Switch to SM timedout");
        fail_test();
    }

    if (!cyclic_success) {
        DEBUG_ERR("Error running cyclic processing");
        fail_test();
    }

    DEBUG_INF("---- TEST 4 PASSED ----");

    DEBUG_INF("Writing original persistent data back to EEPROM");
    MemStoreManager_set_pers_data(ORIG_PERS_DATA);
    MemStoreManager_step();
    DEBUG_INF("ALL TESTS PASSED");

}

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void fail_test(void) {
    DEBUG_INF("Writing original persistent data back to EEPROM");
    MemStoreManager_set_pers_data(ORIG_PERS_DATA);
    MemStoreManager_step();
    DEBUG_ERR("TEST FAILED");
    Debug_exit(1);
}

bool cyclic_processing(void) {
    /* Step Components */
    if (!Eps_step()) {
        DEBUG_ERR("Error stepping Eps");
        return false;
    }

    /* Step apps */
    if (!Power_step()) {
        DEBUG_ERR("Error stepping Power");
        return false;
    }

    /* Step System */
    if (!OpModeManager_step()) {
        DEBUG_ERR("Error stepping OpModeManager");
        return false;
    }

    if (!MemStoreManager_step()) {
        DEBUG_ERR("Error stepping MemStoreManager");
        return false;
    }

    return true;
}

void clean_between_tests(void) {
    memset(&DP.OPMODEMANAGER, 0, sizeof(OpModeManager_Dp));
}