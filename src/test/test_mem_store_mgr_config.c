/**
 * @file test_mem_store_mgr_config.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Performs functionality test of the MemStoreManager's configuration
 * management system.
 *  
 * Task ref: [UT_2.9.3]
 * 
 * Test procedure:
 * 1. Use tool_config_pack to create a new configuration file binary.
 * 2. Use tool_config_flash to flash this binary onto the EEPROM.
 * 3. Flash this test file to the TM4C and allow it to run, the test will
 *    declare itself successful at the end of execution.
 * 
 * This procedure can be completed by running the following script in the root
 * directory of the obc-firmware repo.:
 * ```shell
 * > ./build -c tool_config_pack
 * > ./builds/debug/bin/tool_config_pack.exe config/uos3_config_data_<VERSION>.toml config/uos3_config_data_<VERSION>.bin 
 * > ./build -t launchpad -c --config config/uos3_config_data_<VERSION>.bin --flash tool_config_flash.elf tool_config_flash
 * > ./build -t launchpad -c --tests --flash test_mem_store_mgr_config.elf test_mem_store_mgr_config
 * ```
 * 
 * The test automatically verifies:
 * 1. That the config files when first flashed are all correct.
 * 2. Corrupting a config file (by writing an invalid version number to the
 *    file) will be detected, but will not result in a failure to initialise
 *    the storage manager.
 * 3. Corrupting all config files will fail to init the config manager.
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
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "util/crypto/Crypto_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

void print_cfg_status() {
    DEBUG_INF(
        "CFG file states: %s, %s, %s", 
        DP.MEMSTOREMANAGER.CFG_FILE_1_OK ? "OK" : "CORRUPTED",
        DP.MEMSTOREMANAGER.CFG_FILE_2_OK ? "OK" : "CORRUPTED",
        DP.MEMSTOREMANAGER.CFG_FILE_3_OK ? "OK" : "CORRUPTED"
    );
}

/* Note: until MemStoreManager is implemented this function will test the
 * EEPROM driver */
int main(void) {
    ErrorCode eeprom_error = ERROR_NONE;

    /* Init system critical modules */
    Kernel_init_critical_modules();

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

    DEBUG_INF(
        "Performing first initialisation to test all config files correct"
    );

    /* Init the MemStoreManager */
    if (!MemStoreManager_init()) {
        DEBUG_ERR("Init failed when not expected, test failed!");
        print_cfg_status();
        Debug_exit(1);
    }
    DEBUG_INF("MemStoreManager initialised");
    print_cfg_status();

    /* Save the config data for use in the update file */
    MemStoreManager_ConfigData cfg_data = CFG;

    /* Corrupt the CFG file */
    uint32_t corrupted_data = 0xFFFFFFFF;
    eeprom_error = Eeprom_write(
        EEPROM_ADDR_CFG_FILE_1,
        &corrupted_data,
        4
    );
    if (eeprom_error != ERROR_NONE) {
        DEBUG_ERR("EEPROM error while trying to corrupt CFG file");
        Debug_exit(1);
    }

    /* Init the MemStoreManager */
    if (!MemStoreManager_init()) {
        DEBUG_ERR("Init failed when not expected, test failed!");
        print_cfg_status();
        Debug_exit(1);
    }
    DEBUG_INF("MemStoreManager initialised");
    print_cfg_status();

    /* Corrupt the CFG file */
    eeprom_error = Eeprom_write(
        EEPROM_ADDR_CFG_FILE_2,
        &corrupted_data,
        4
    );
    if (eeprom_error != ERROR_NONE) {
        DEBUG_ERR("EEPROM error while trying to corrupt CFG file");
        Debug_exit(1);
    }

    /* Init the MemStoreManager */
    if (!MemStoreManager_init()) {
        DEBUG_ERR("Init failed when not expected, test failed!");
        print_cfg_status();
        Debug_exit(1);
    }
    DEBUG_INF("MemStoreManager initialised");
    print_cfg_status();

    /* Corrupt the CFG file */
    eeprom_error = Eeprom_write(
        EEPROM_ADDR_CFG_FILE_3,
        &corrupted_data,
        4
    );
    if (eeprom_error != ERROR_NONE) {
        DEBUG_ERR("EEPROM error while trying to corrupt CFG file");
        Debug_exit(1);
    }

    /* Init the MemStoreManager */
    if (!MemStoreManager_init()) {
        DEBUG_ERR("Init failed when not expected, test failed!");
        print_cfg_status();
        Debug_exit(1);
    }
    DEBUG_INF("MemStoreManager init passed as expected");
    print_cfg_status();
    if (!DP.MEMSTOREMANAGER.CFG_FILE_1_OK 
        &&
        !DP.MEMSTOREMANAGER.CFG_FILE_2_OK
        &&
        !DP.MEMSTOREMANAGER.CFG_FILE_3_OK 
    ) {
        DEBUG_INF("All files are corrupted but config init passed, inidcating backup file loaded correctly");
    }

    DEBUG_INF("Corrupted config files were detected, performing update");

    /* Build new config file */
    MemStoreManager_ConfigFile cfg_file;
    memset((void *)&cfg_file, 0, sizeof(MemStoreManager_ConfigFile));
    cfg_file.data = cfg_data;

    /* Calculate CRC */
    Crypto_get_crc32(
        (uint8_t *)&cfg_file.data,
        sizeof(MemStoreManager_ConfigData),
        &cfg_file.crc
    );

    /* Perform update */
    if (!MemStoreManager_config_update(&cfg_file)) {
        DEBUG_ERR("CFG file update failed, test failed!");
        Debug_exit(1);
    }
    DEBUG_INF("CFG update succeeded");

    /* Re-init module */
    if (!MemStoreManager_init()) {
        DEBUG_ERR("Init failed when not expected, test failed!");
        print_cfg_status();
        Debug_exit(1);
    }
    DEBUG_INF("MemStoreManager initialised");
    print_cfg_status();
    MemStoreManager_debug_print_cfg();
    MemStoreManager_debug_print_pers();

    DEBUG_INF("Test passed!");

    /* Destroy the event manager */
    EventManager_destroy();
}