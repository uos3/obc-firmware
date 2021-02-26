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
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "components/eps/Eps_public.h"
#include "applications/mission/Mission_public.h"
#include "applications/power/Power_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
    ErrorCode eeprom_error = ERROR_NONE;
    bool run_loop = true;

    /* Init system critical modules */
    if (!DataPool_init()) {
        Debug_exit(1);
    }
    if (!Board_init()) {
        Debug_exit(2);
    }
    if (!Debug_init()) {
        Debug_exit(3);
    }
    if (!EventManager_init()) {
        Debug_exit(4);
    }
    if (Eeprom_init() != ERROR_NONE) {
        Debug_exit(5);
    }
    if (!MemStoreManager_init()) {
        Debug_exit(6);
    }

    DEBUG_INF("---- Power Test ----");
    DEBUG_INF("Basic initialisation complete");

    /* Init the EPS */
    if (!Eps_init()) {
        Debug_exit(7);
    }
    DEBUG_INF("Eps initialised");

    /* Init the mission app */
    if (!Mission_init()) {
        Debug_exit(8);
    }
    DEBUG_INF("Mission initialised");

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

        if (!Mission_step()) {
            DEBUG_ERR(
                "Mission_step() failed! DP.MISSION.ERROR_CODE = 0x%04X",
                DP.MISSION.ERROR_CODE
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
        if (!EventManager_cleanup_events()) {
            DEBUG_ERR(
                "EventManager_cleanup_events() failed! DP.EVENTMANAGER.ERROR_CODE = 0x%04X",
                DP.EVENTMANAGER.ERROR_CODE
            );
            Debug_exit(13);
        }

        /* If no events "sleep" */
        if (DP.EVENTMANAGER.NUM_RAISED_EVENTS == 0) {
            DEBUG_INF("No events, sleeping");
            run_loop = false;
        }

    }

    /* Cleanup */
    EventManager_destroy();

    return EXIT_SUCCESS;
}