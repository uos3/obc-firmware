/**
 * @file test_power.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application main test executable
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-02-26
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/rtc/Rtc_public.h"
#include "drivers/board/Board_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "drivers/timer/Timer_public.h"
#include "drivers/udma/Udma_public.h"
#include "drivers/uart/Uart_public.h"
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "system/opmode_manager/OpModeManager_public.h"
#include "components/eps/Eps_public.h"
#include "applications/power/Power_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
    ErrorCode error = ERROR_NONE;
    bool run_loop = true;
    uint8_t test_step = 0;
    Rtc_Timestamp timeout_start;
    Rtc_Timespan timeout_timespan;
    bool timeout_passed;

    /* Init system critical modules */
    Kernel_init_critical_modules();
    if (Rtc_init() != ERROR_NONE) {
        Debug_exit(1);
    }
    if (Eeprom_init() != ERROR_NONE) {
        Debug_exit(1);
    }
    if (!MemStoreManager_init()) {
        Debug_exit(1);
    }
    error = Timer_init();
    if (error != ERROR_NONE) {
        DEBUG_ERR("Timer init error 0x%04X", error);
        Debug_exit(1);
    }

    /* Set the timespan for the HK data collection timeout */
    timeout_timespan = Rtc_timespan_from_ms(
        CFG.POWER_TASK_TIMER_DURATION_S * 1000
    );

    DEBUG_INF("---- Power Test ----");
    DEBUG_INF("Basic initialisation complete");

    /* Init UDMA and UART */
    if (Udma_init() != ERROR_NONE) {
        DEBUG_ERR("Couldn't init uDMA");
        Debug_exit(1);
    }
    if (Uart_init() != ERROR_NONE) {
        DEBUG_ERR("Couldn't init Uart");
        Debug_exit(1);
    }

    /* Init the EPS */
    if (!Eps_init()) {
        Debug_exit(1);
    }
    DEBUG_INF("Eps initialised");

    /* Init the opmode manager */
    if (!OpModeManager_init()) {
        Debug_exit(1);
    }
    DEBUG_INF("OpModeManager initialised");

    /* Init the power app */
    if (!Power_init()) {
        Debug_exit(1);
    }
    DEBUG_INF("Power initialised");

    DEBUG_INF("---- TEST 1: OPMODE TRANSITIONS ----");

    /* Enter main exec loop */
    while (run_loop) {

        /* ---- DRIVERS ---- */
        
        if (!Uart_step()) {

        }

        /* ---- COMPONENTS ---- */
        
        if (!Eps_step()) {
            char error_chain[64] = {0};
            Kernel_error_to_string(&DP.EPS.ERROR, error_chain);
            DEBUG_ERR(
                "Eps_step() failed! Error chain = %s", 
                error_chain
            );

            /* If the root cause is a timeout we will actually not exit here,
             * since Power is supposed to handle these */
            if (DP.EPS.ERROR.code != EPS_ERROR_COMMAND_TIMEOUT) {
                Debug_exit(1);
            }
            else {
                DEBUG_INF("Continuing to allow Power to handle timeout");
            }
        }

        /* ---- APPLICATIONS ---- */

        if (!OpModeManager_step()) {
            char error_chain[64] = {0};
            Kernel_error_to_string(&DP.OPMODEMANAGER.ERROR, error_chain);
            DEBUG_ERR(
                "OpModeManager_step() failed! Error chain = %s",
                error_chain
            );
            Debug_exit(1);
        }

        if (!Power_step()) {
            char error_chain[64] = {0};
            Kernel_error_to_string(&DP.POWER.ERROR, error_chain);
            DEBUG_ERR(
                "Power_step() failed! Error chain = %s",
                error_chain
            );
            Debug_exit(1);
        }

        /* Cycle clean up */
        EventManager_cleanup_events();

        /* ---- TEST SEQUENCE ---- */
        switch (test_step) {
            /* Stepping through different opmodes */
            case 0:
                /* If there's no events raised at the end of the last cycle
                 * move to the next opmode */
                if (DP.EVENTMANAGER.NUM_RAISED_EVENTS == 0) {
                    if (DP.OPMODEMANAGER.OPMODE 
                        < OPMODEMANAGER_OPMODE_PICTURE_TAKING
                    ) {                
                        
                        /* Trigger OpMode transition if not already in one */
                        if (DP.OPMODEMANAGER.NEXT_OPMODE 
                            == DP.OPMODEMANAGER.OPMODE
                        ) {
                            DP.OPMODEMANAGER.NEXT_OPMODE 
                                = DP.OPMODEMANAGER.OPMODE + 1;
                            DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE = true;

                            DEBUG_INF(
                                "No events, setting OPMODE to %d", 
                                DP.OPMODEMANAGER.NEXT_OPMODE
                            );
                        }
                    }
                    else {
                        DEBUG_INF("Final OPMODE reached, moving to next step");
                        test_step++;
                    }
                }
                break;
            /* Transition back to NF mode */
            case 1:
                DEBUG_INF("Transition back to NF for next test");
                DP.OPMODEMANAGER.NEXT_OPMODE 
                    = OPMODEMANAGER_OPMODE_NOMINAL_FUNCTIONING;
                DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE = true;
                test_step++;
                break;
            case 2:
                if (DP.OPMODEMANAGER.OPMODE 
                    == 
                    OPMODEMANAGER_OPMODE_NOMINAL_FUNCTIONING
                ) {
                    test_step++;
                    DEBUG_INF("---- TEST 2: HK DATA COLLECTION ----");
                    DEBUG_INF("Waiting for POWER to collect EPS HK data");
                    
                    /* Record the current time so we have a timeout on the data
                     * collection */
                    timeout_start = Rtc_get_timestamp();
                }
                break;
            /* Wait for EPS HK data */
            case 3:
                /* Check timeout of collection */
                if (Rtc_is_timespan_ellapsed(
                    &timeout_start,
                    &timeout_timespan,
                    &timeout_passed
                ) != ERROR_NONE) {
                    DEBUG_ERR("Couldn't get RTC timeout passed");
                    Debug_exit(1);
                }
                if (timeout_passed) {
                    DEBUG_ERR("HK collection timeout passed");
                    Debug_exit(1);
                }

                /* Check for collection starting */
                if (EventManager_is_event_raised(EVT_EPS_NEW_HK_DATA)) {
                    DEBUG_INF("New HK data collected");
                    test_step++;
                }
                break;
            case 4:
            /* End of test */
            default:
                DEBUG_INF("---- ALL TESTS PASSED ----");
                run_loop = false;
        }
    }

    /* Cleanup */
    EventManager_destroy();

    return EXIT_SUCCESS;
}