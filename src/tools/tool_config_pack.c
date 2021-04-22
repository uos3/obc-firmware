/**
 * @file tool_config_pack.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Pack a config TOML file into a configuration binary file.
 * @version 0.1
 * @date 2020-12-16
 * 
 * This tool is used to create binary configuration files from raw data source
 * files. See the UoS3_ObcSoftwareConfigurationManual in the SharePoint for 
 * more information. 
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* System includes */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* External includes */
#include <toml.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "util/crypto/Crypto_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(int argc, char **pp_argv) {

    /* Init the debug system */
    if (!Debug_init()) {
        Debug_exit(1);
    }

    /* Check that the right number of arguments is provided */
    if (argc != 3) {
        DEBUG_ERR(
            "Wrong number of arguments, use tool_config_pack <PATH_TO_CONFIG_TOML> <PATH_TO_OUTPUT_BINARY>"
        );
        Debug_exit(1);
    }

    /* Error buffer */
    char err_buffer[200];

    /* Get the endianness of the system */
    uint32_t i = 1;
    char *p_c = (char *)&i;
    bool is_little_endian = (bool)*p_c;

    /* Open the config toml file */
    FILE *fp_toml = fopen(pp_argv[1], "r");
    if (fp_toml == NULL) {
        DEBUG_ERR("Could not load TOML file from %s", pp_argv[1]);
        Debug_exit(1);
    }

    /* Parse the toml file */
    toml_table_t *p_config = toml_parse_file(
        fp_toml, 
        err_buffer, 
        sizeof(err_buffer)
    );

    /* Close the opened toml file */
    fclose(fp_toml);

    /* Check for errors parsing the file */
    if (p_config == NULL) {
        DEBUG_ERR("Could not parse TOML file: %s", err_buffer);
        Debug_exit(1);
    }

    DEBUG_INF("TOML file parsed");

    /* Empty config data */
    MemStoreManager_ConfigData cfg_data;
    memset((void *)&cfg_data, 0, sizeof(MemStoreManager_ConfigData));
    bool cfg_ok = true;

    /* Set all components of the cfg data */
    toml_datum_t version = toml_int_in(p_config, "VERSION");
    if (!version.ok) {
        DEBUG_ERR("Missing TOML parameter: VERSION");
        cfg_ok = false;
    }
    else {
        cfg_data.VERSION = (uint8_t)version.u.i;
    }

    toml_datum_t power_task_timer_duration_s = toml_int_in(
        p_config, 
        "POWER_TASK_TIMER_DURATION_S"
    );
    if (!power_task_timer_duration_s.ok) {
        DEBUG_ERR("Missing TOML parameter: POWER_TASK_TIMER_DURATION_S");
        cfg_ok = false;
    }
    else {
        cfg_data.POWER_TASK_TIMER_DURATION_S 
            = (uint16_t)power_task_timer_duration_s.u.i;
    }

    toml_datum_t power_op_mode_ocp_rail_config = toml_int_in(
        p_config, 
        "POWER_OPMODE_OCP_STATE_CONFIG"
    );
    if (!power_op_mode_ocp_rail_config.ok) {
        DEBUG_ERR("Missing TOML parameter: POWER_OPMODE_OCP_STATE_CONFIG");
        cfg_ok = false;
    }
    else {
        cfg_data.POWER_OPMODE_OCP_STATE_CONFIG 
            = (uint32_t)power_op_mode_ocp_rail_config.u.i;
    }

    toml_datum_t eps_reset_rail_after_ocp = toml_int_in(
        p_config, 
        "EPS_RESET_RAIL_AFTER_OCP"
    );
    if (!eps_reset_rail_after_ocp.ok) {
        DEBUG_ERR("Missing TOML parameter: EPS_RESET_RAIL_AFTER_OCP");
        cfg_ok = false;
    }
    else {
        cfg_data.EPS_RESET_RAIL_AFTER_OCP 
            = (uint8_t)eps_reset_rail_after_ocp.u.i;
    }

    toml_datum_t eps_tobc_timer_length = toml_int_in(
        p_config, 
        "EPS_TOBC_TIMER_LENGTH"
    );
    if (!eps_tobc_timer_length.ok) {
        DEBUG_ERR("Missing TOML parameter: EPS_TOBC_TIMER_LENGTH");
        cfg_ok = false;
    }
    else {
        cfg_data.EPS_TOBC_TIMER_LENGTH 
            = (uint16_t)eps_tobc_timer_length.u.i;
    }

    toml_array_t *p_opmode_appid_table = toml_array_in(
        p_config,
        "OPMODE_APPID_TABLE"
    );
    if (!p_opmode_appid_table) {
        DEBUG_ERR("Missing TOML parameter: OPMODE_APPID_TABLE");
        cfg_ok = false;
    }
    else {
        for (int mode = 0; mode < OPMODEMANAGER_NUM_OPMODES; ++mode) {
            toml_array_t *p_mode_appid_array = toml_array_at(
                p_opmode_appid_table,
                mode
            );
            if (!p_mode_appid_array) {
                DEBUG_ERR("Missing APPID array for OPMODE %d", mode);
                cfg_ok = false;
            }
            else {
                for (int i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
                    toml_datum_t app_id = toml_int_at(p_mode_appid_array, i);
                    if (!app_id.ok) {
                        DEBUG_ERR("Invalid APPID[%d] for OPMODE %d", i, mode);
                        cfg_ok = false;
                    }
                    else {
                        cfg_data.OPMODE_APPID_TABLE[mode][i] 
                            = (Kernel_AppId)app_id.u.i;
                    }
                }
            }
        }
    }

    /* Exit if config loading didn't work */
    if (!cfg_ok) {
        DEBUG_ERR("Missing TOML parameters, cannot pack config");
        Debug_exit(1);
    }

    DEBUG_INF("All parameters in TOML file, packing");

    /* Free the TOML file */
    toml_free(p_config);

    /* Create the config file */
    MemStoreManager_ConfigFile cfg_file;
    memset((void *)&cfg_file, 0, sizeof(MemStoreManager_ConfigFile));
    cfg_file.data = cfg_data;
    
    /* Calculate the CRC */
    Crypto_get_crc32(
        (uint8_t *)&cfg_file.data, 
        sizeof(MemStoreManager_ConfigData),
        &cfg_file.crc  
    );

    DEBUG_INF(
        "Packing complete, config file is %d bytes in size", 
        sizeof(MemStoreManager_ConfigFile)
    );
    DEBUG_INF("Writing config file");

    /* Create a new file with the proper name */
    FILE *fp_binary = fopen(pp_argv[2], "w+b");
    if (fp_binary == NULL) {
        DEBUG_ERR("Cannot create binary file at %s", pp_argv[2]);
        Debug_exit(1);
    }

    /* Write out the binary file */
    size_t write_size = fwrite(
        (void *)&cfg_file, 
        sizeof(MemStoreManager_ConfigFile), 
        1,
        fp_binary    
    );
    if (write_size != 1) {
        DEBUG_ERR("Error writing binary file out to %s", pp_argv[2]);
        Debug_exit(1);
    }

    /* Close the file */
    fclose(fp_binary);

    DEBUG_INF("Binary config file written to %s", pp_argv[2]);

    return EXIT_SUCCESS;
}