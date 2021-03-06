/**
 * @file obc_firmware_main.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Main entry point of the OBC software.
 * 
 * @version 0.1
 * @date 2021-03-11
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "util/debug/Debug_public.h"
#include "obc_firmware/obc_firmware.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {

    /* ---- INITIALISATION ---- */

    /* Perform the early init */
    obc_firmware_init_early_modules();

    /* Do system init */
    obc_firmware_init_system();
    
    /* Do component init */
    obc_firmware_init_components();

    /* Do application init */
    obc_firmware_init_applications();

    DEBUG_INF("obc_firmware init complete");
    DEBUG_INF("---- OBC-FIRMWARE MAIN LOOP ----");

    /* ---- MAIN LOOP ---- */
    while (1) {

        /* First thing in the loop is to step the drivers */
        obc_firmware_step_drivers();

        /* Then the components */
        obc_firmware_step_components();

        /* Then the system, which inclues the applications through system apps
         * and user apps */
        obc_firmware_step_system();

        /* Clean up events */
        EventManager_cleanup_events();

        /* If no events after cleanup wait until interrupt occurs (only on
         * TM4C, don't have propper interrupts on linux */
        Kernel_disable_interrupts();
        if (DP.EVENTMANAGER.NUM_RAISED_EVENTS == 0) {
            #ifdef TARGET_TM4C
            DEBUG_INF("No events, waiting for interrupt...");
            __asm("WFI");
            #endif
        }
        Kernel_enable_interrupts();
    }

    return 0;
}