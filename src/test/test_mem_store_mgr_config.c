/**
 * @file test_mem_store_mgr_config.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Performs functionality test of the MemStoreManager's configuration
 * management system.
 * 
 * @version 0.1
 * @date 2020-12-08
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/eeprom/Eeprom_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

/* Note: until MemStoreManager is implemented this function will test the
 * EEPROM driver */
int main() {
    ErrorCode eeprom_error = ERROR_NONE;

    /* Init system critical modules */
    if (!DataPool_init()) {
        Debug_exit(1);
    }
    if (!Board_init()) {
        Debug_exit(1);
    }
    if (!Debug_init()) {
        Debug_exit(1);
    }
    if (!EventManager_init()) {
        Debug_exit(1);
    }

    DEBUG_INF("MemStoreManager Configuration Test");
    DEBUG_INF("Basic initialisation complete");

    /* Init the EEPROM driver */
    eeprom_error = Eeprom_init();
    if (eeprom_error != ERROR_NONE) {
        Debug_exit(1);
    }
    DEBUG_INF("EEPROM init successful");

    /* Perform the EEPROM self test */
    eeprom_error = Eeprom_self_test();
    if (eeprom_error != ERROR_NONE) {
        Debug_exit(1);
    }
    DEBUG_INF("EEPROM self test successful");

    /* Write some data to the EEPROM */
    uint32_t data[2] = {0x12345678, 0x56789ABC};
    eeprom_error = Eeprom_write(data, 0x100, sizeof(data));
    if (eeprom_error != ERROR_NONE) {
        Debug_exit(1);
    }

    /* Read that data back */
    uint32_t read_data[2] = {0};
    eeprom_error = Eeprom_read(read_data, 0x100, sizeof(data));
    if (eeprom_error != ERROR_NONE) {
        Debug_exit(1);
    }

    DEBUG_INF(
        "Wrote {0x08X, 0x08X} to EEPROM, read {0x08X, 0x08X}",
        data[0],
        data[1],
        read_data[0],
        read_data[1]    
    );

    /* Destroy the event manager */
    EventManager_destroy();
}