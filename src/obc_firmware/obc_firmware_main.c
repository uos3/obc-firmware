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

#include "system/event_manager/EventManager_public.h"
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
        if (!EventManager_cleanup_events()) {
            /* TODO: register error with FDIR */
        }

        /* TODO: if no events after cleanup request RTC to enter sleep mode */
    }

    return 0;
}