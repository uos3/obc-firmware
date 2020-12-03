/**
 * @ingroup i2c
 * 
 * @file I2c_public_linux.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief I2C driver for the firmware.
 * 
 * This file implements the I2C driver for linux targets.
 * 
 * Note: This driver is currently backed by a dummy which only echos
 * information to the terminal, all functions execute successfully.
 * 
 * See I2c_public.h for more information.
 * 
 * @version 0.1
 * @date 2020-11-12
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/i2c/I2c_public.h"
#include "drivers/i2c/I2c_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode I2c_init(uint32_t *p_modules_in, size_t num_modules_in) {
    /* Mark these as unused */
    (void) p_modules_in;
    (void) num_modules_in;

    /* Set the initialised flag to true */
    I2C.initialised = true;

    return ERROR_NONE;
}

ErrorCode I2c_step(void) {
    /* Won't actually do anything in this as all the functions will succeed and
     * return the correct values immediately */

    return ERROR_NONE;
}

ErrorCode I2c_device_send_bytes(
    I2c_Device *p_device_in, 
    uint8_t *p_data_in, 
    size_t length_in
) {
    /* Alloc string to print bytes in hex, 2 chars ber pyte, + spaces, + null */
    char *p_hex_string = (char *)malloc(sizeof(char) * 3 * length_in);
    char buf[4] = {0};

    /* Print bytes except last into the string */
    for (int i = 0; i < length_in - 1; ++i) {
        sprintf((char *)buf, "%02X ", p_data_in[i]);
        strcat(p_hex_string, (char *)buf);
    }

    sprintf((char *)buf, "%02X", p_data_in[length_in - 1]);
    strcat(p_hex_string, (char *)buf);

    DEBUG_DBG(
        "I2C send (%02X, %02X): %s", 
        p_device_in->module, 
        p_device_in->address,
        p_hex_string
    );

    free(p_hex_string);

    /* Raise action finished */
    if (!EventManager_raise_event(EVT_I2C_ACTION_FINISHED)) {
        DEBUG_ERR("Couldn't raise EVT_I2C_ACTION_FINISHED");
        return I2C_ERROR_EVENTMANAGER_ERROR;
    }

    return ERROR_NONE;
}

ErrorCode I2c_device_recv_bytes(
    I2c_Device *p_device_in,
    uint8_t reg_in,
    size_t length_in
) {
    /* For the linux dummy we're going to pretend and will recieve some random
     * data instead */
    bool empty_found = false;
    for (int i = 0; i < I2C_MAX_NUM_ACTIONS; ++i) {
        if (I2C.action_types[i] == I2C_ACTION_TYPE_NONE) {
            I2C.u_actions[i].burst_recv.device = *p_device_in;
            I2C.u_actions[i].burst_recv.length = length_in;

            I2C.action_types[i] = I2C_ACTION_TYPE_BURST_RECV;

            empty_found = true;

            break;
        }
    }

    if (empty_found) {
        /* Raise finished event since the work is done in get_recved_bytes */
        if (!EventManager_raise_event(EVT_I2C_ACTION_FINISHED)) {
            DEBUG_ERR("Error raising EVT_I2C_ACTION_FINISHED");
            return I2C_ERROR_EVENTMANAGER_ERROR;
        }

        return ERROR_NONE;
    }
    else {
        DEBUG_ERR("Max number of I2C actions reached, cannot raise another one");
        return I2C_ERROR_MAX_ACTIONS_REACHED;
    }
}

ErrorCode I2c_get_device_recved_bytes(
    I2c_Device *p_device_in,
    uint8_t *p_bytes_out
) {
    /* In this linux dummy we read the required number of bytes from
     * /dev/urandom (random bytes) and output that */
    bool dev_found = false;
    #if DEBUG_MODE
    size_t length = 0;
    #endif
    for (int i = 0; i < I2C_MAX_NUM_ACTIONS; ++i) {
        if (I2C.action_types[i] == I2C_ACTION_TYPE_BURST_RECV) {
            /* If the devices don't match skip to next one */
            if (!I2c_devices_equal(
                p_device_in, 
                &I2C.u_actions[i].burst_recv.device
            )) {
                continue;
            }

            dev_found = true;

            /* Read random bytes of given length */
            FILE *fp_random = fopen("/dev/urandom", "r");
            if (fp_random == NULL) {
                DEBUG_ERR("Couldn't open /dev/urandom");
                /* This obivously isn't the right error code but we're not
                 * going to add separate ones for linux as it's not official */
                return I2C_ERROR_ZERO_LENGTH_RECEIVE;
            }

            #if DEBUG_MODE
            length = I2C.u_actions[i].burst_recv.length;
            #endif

            size_t num_items = fread(
                p_bytes_out, 
                I2C.u_actions[i].burst_recv.length,
                1,
                fp_random
            );
            if (num_items != 1) {
                DEBUG_ERR(
                    "Could not get 1 items from /dev/urandom (read %d instead)",
                    num_items
                );
                /* This obivously isn't the right error code but we're not
                 * going to add separate ones for linux as it's not official */
                return I2C_ERROR_ZERO_LENGTH_RECEIVE;
            }

            /* Close the file */
            fclose(fp_random);

            break;
        }
    }

    if (!dev_found) {
        DEBUG_ERR("Could not find read action on device");
        return I2C_ERROR_NO_ACTION_FOR_DEVICE;
    }

    #if DEBUG_MODE
    char *p_bytes_string = (char *)malloc(4 * length);
    memset(p_bytes_string, 0, 4 * length);
    char buf[4];
    for (int i = 0; i < length; ++i) {
        sprintf((char *)buf, "%02X ", p_bytes_out[i]);
        strcat(p_bytes_string, buf);
    }
    DEBUG_DBG(
        "I2C recv (%02X, %02X): %s", 
        p_device_in->module, 
        p_device_in->address,
        p_bytes_string
    );
    free(p_bytes_string);
    #endif

    return ERROR_NONE;
}

ErrorCode I2c_get_device_action_status(
    I2c_Device *p_device_in,
    I2c_ActionStatus *p_status_out
) {
    /* Return success */
    *p_status_out = I2C_ACTION_STATUS_SUCCESS;

    return ERROR_NONE;
}

ErrorCode I2c_get_device_action_failure_cause(
    I2c_Device *p_device_in,
    ErrorCode *p_error_out
) {
    /* Since we don't track device action failure causes in the linux dummy
     * just return none */
    *p_error_out = ERROR_NONE;

    return ERROR_NONE;
}

ErrorCode I2c_clear_device_action(I2c_Device *p_device_in) {
    for (int i = 0; i < I2C_MAX_NUM_ACTIONS; ++i) {
        I2c_ActionType type = I2C.action_types[i];
        bool devices_match = false;

        switch (type) {
            case I2C_ACTION_TYPE_NONE:
                continue;

            case I2C_ACTION_TYPE_SINGLE_SEND:
                /* If devices match */
                devices_match = I2c_devices_equal(
                    p_device_in, &I2C.u_actions[i].single_send.device
                );

                break;

            case I2C_ACTION_TYPE_SINGLE_RECV:
                /* If devices match */
                devices_match = I2c_devices_equal(
                    p_device_in, &I2C.u_actions[i].single_recv.device
                );

                break;

            case I2C_ACTION_TYPE_BURST_SEND:
                /* If devices match */
                devices_match = I2c_devices_equal(
                    p_device_in, &I2C.u_actions[i].burst_send.device
                );

                break;

            case I2C_ACTION_TYPE_BURST_RECV:
                /* If devices match */
                devices_match = I2c_devices_equal(
                    p_device_in, &I2C.u_actions[i].burst_recv.device
                );

                break;
        }

        /* Set all bytes in the field to 0 if the devices match */
        if (devices_match = true) {
            memset(&I2C.u_actions[i], 0, sizeof(I2c_Action));
            /* Set the type to none */
            I2C.action_types[i] = I2C_ACTION_TYPE_NONE;
            
            /* Break out of the loop */
            break;
        }
    }

    return ERROR_NONE;
}