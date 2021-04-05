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
#include "drivers/board/Board_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "drivers/timer/Timer_public.h"
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

    /* Init system critical modules */
    Kernel_init_critical_modules();
    if (Eeprom_init() != ERROR_NONE) {
        Debug_exit(5);
    }
    if (!MemStoreManager_init()) {
        Debug_exit(6);
    }
    error = Timer_init();
    if (error != ERROR_NONE) {
        DEBUG_ERR("Timer init error 0x%04X", error);
        Debug_exit(1);
    }

    DEBUG_INF("---- Power Test ----");
    DEBUG_INF("Basic initialisation complete");

    /* Init the EPS */
    if (!Eps_init()) {
        Debug_exit(7);
    }
    DEBUG_INF("Eps initialised");

    /* Init the opmode manager */
    if (!OpModeManager_init()) {
        Debug_exit(8);
    }
    DEBUG_INF("OpModeManager initialised");

    /* Init the power app */
    if (!Power_init()) {
        Debug_exit(9);
    }
    DEBUG_INF("Power initialised");

    /* Enter main exec loop */
    while (run_loop) {

        DEBUG_TRC("-- CYCLE START --");

        /* ---- DRIVERS ---- */
        
        /* TODO: uart */

        /* ---- COMPONENTS ---- */
        
        if (!Eps_step()) {
            DEBUG_ERR(
                "Eps_step() failed! DP.EPS.ERROR_CODE = 0x%04X", 
                DP.EPS.ERROR_CODE
            );
            Debug_exit(10);
        }

        /* ---- APPLICATIONS ---- */

        if (!OpModeManager_step()) {
            DEBUG_ERR(
                "OpModeManager_step() failed! DP.OPMODEMANAGER.ERROR_CODE = 0x%04X",
                DP.OPMODEMANAGER.ERROR_CODE
            );
            Debug_exit(11);
        }

        if (!Power_step()) {
            DEBUG_ERR(
                "Power_step() failed! DP.POWER.ERROR_CODE = 0x%04X",
                DP.POWER.ERROR_CODE
            );
            Debug_exit(12);
        }

        /* Cycle clean up */
        EventManager_cleanup_events();

        /* If no move to next mode, to test mode switching */
        if (DP.EVENTMANAGER.NUM_RAISED_EVENTS == 0) {
            if (DP.OPMODEMANAGER.OPMODE < OPMODEMANAGER_OPMODE_PICTURE_TAKING) {                
                
                /* Trigger OpMode transition if not already in one */
                if (DP.OPMODEMANAGER.NEXT_OPMODE == DP.OPMODEMANAGER.OPMODE) {
                    DP.OPMODEMANAGER.NEXT_OPMODE = DP.OPMODEMANAGER.OPMODE + 1;
                    DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE = true;

                    DEBUG_INF(
                        "No events, setting OPMODE to %d", 
                        DP.OPMODEMANAGER.NEXT_OPMODE
                    );
                }
            }
            else {
                DEBUG_INF("Final OPMODE reached, exiting");
                run_loop = false;
            }
        }

    }

    DEBUG_INF("---- TEST PASSED ----");

    /* Cleanup */
    EventManager_destroy();

    return EXIT_SUCCESS;
}