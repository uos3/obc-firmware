/**
 * @ingroup demo
 * 
 * @file demo_imu.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Short demo designed to test the IMU capabilities. 
 * @version 0.1
 * @date 2020-10-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup demo_imu Demo Imu
 * @{
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "components/imu/Imu_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
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

    DEBUG_INF("IMU Demo");

    /* Init the I2C driver */
    uint32_t i2c_modules[] = {0x02};
    ErrorCode i2c_error = I2c_init((uint32_t *)i2c_modules, 1);
    if (i2c_error != ERROR_NONE) {
        DEBUG_ERR("I2C error code: %d", i2c_error);
        Debug_exit(1);
    }

    /* Init the IMU */
    if (!Imu_init()) {
        DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR_CODE);
        Debug_exit(1);
    }

    DEBUG_INF("IMU initialised");

    DEBUG_INF("Begining main loop\n");

    DEBUG_INF("State table:");
    DEBUG_INF(
        "| STATE | SUBSTATE | COMMAND | ERROR | EVENTS"
    );
    #ifdef DEBUG_MODE
    char *p_table_format = 
        "|  0x%02X |     0x%02X |    0x%02X |  0x%02X | %s";
    #endif

    /* 
     * Demo step is a counter used to:
     * 0. Wait for IMU gyro offsets to be set
     * 1. Start read temperature
     * 2. Wait for read complete 
     * 3. Start read gyroscope
     * 4. Wait for read complete
     * 5. Start read magnetometer
     * 6. Wait for reac complete & print the data out
     */
    uint8_t demo_step = 0;

    /* "Main loop", used to simulate the spacecraft's loop */
    bool run_loop = true;
    bool sleep = false;
    while (run_loop) {

        DEBUG_TRC("--- CYCLE ---");

        /* Do the demo actions */
        bool event_raised = false;
        switch (demo_step) {
            /* Wait for the set gyro offsets command to finish */
            case 0:
                /* See if success */
                if (!EventManager_poll_event(
                    EVT_IMU_SET_GYRO_OFFSETS_SUCCESS, 
                    &event_raised
                )) {
                    Debug_exit(1);
                }

                /* If success */
                if (event_raised) {
                    /* Print temp */
                    DEBUG_INF("Gyro offsets set");

                    /* Next step */
                    demo_step++;
                }
                /* If not success */
                else {
                    /* See if failed */
                    if (!EventManager_poll_event(
                        EVT_IMU_SET_GYRO_OFFSETS_FAILURE, 
                        &event_raised
                    )) {
                        Debug_exit(1);
                    }

                    /* If failed print error and exit */
                    if (event_raised) {
                        DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR_CODE);
                        Debug_exit(1);
                    }
                    /* Otherwise the read just isn't finished yet, so wait a
                     * bit longer */
                }
                break;

            /* Issue read temp command */
            case 1:
                DEBUG_INF("Issuing READ_TEMPERATURE command");
                if (!Imu_new_command(IMU_CMD_READ_TEMPERATURE)) {
                    Debug_exit(1);
                }
                demo_step++;
                break;

            /* Wait for read complete */
            case 2:
                /* See if success */
                if (!EventManager_poll_event(
                    EVT_IMU_READ_TEMP_SUCCESS, 
                    &event_raised
                )) {
                    Debug_exit(1);
                }

                /* If success */
                if (event_raised) {
                    /* Print temp */
                    DEBUG_INF("temp: %d", DP.IMU.TEMPERATURE_DATA);

                    /* Next step */
                    demo_step++;
                }
                /* If not success */
                else {
                    /* See if failed */
                    if (!EventManager_poll_event(
                        EVT_IMU_READ_TEMP_FAILURE, 
                        &event_raised
                    )) {
                        Debug_exit(1);
                    }

                    /* If failed print error and exit */
                    if (event_raised) {
                        DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR_CODE);
                        Debug_exit(1);
                    }
                    /* Otherwise the read just isn't finished yet, so wait a
                     * bit longer */
                }
                break;

            /* Issue read gyro command */
            case 3:
                DEBUG_INF("Issuing READ_GYROSCOPE command");
                if (!Imu_new_command(IMU_CMD_READ_GYROSCOPE)) {
                    Debug_exit(1);
                }
                demo_step++;
                break;
            
            /* Wait for read complete */
            case 4:
                /* See if success */
                if (!EventManager_poll_event(
                    EVT_IMU_READ_GYRO_SUCCESS, 
                    &event_raised
                )) {
                    Debug_exit(1);
                }

                /* If success */
                if (event_raised) {
                    /* Print gyro data */
                    DEBUG_INF("gyro valid: %d", DP.IMU.GYROSCOPE_DATA_VALID);
                    DEBUG_INF("gyro x: %d", DP.IMU.GYROSCOPE_DATA.x);
                    DEBUG_INF("gyro y: %d", DP.IMU.GYROSCOPE_DATA.y);
                    DEBUG_INF("gyro z: %d", DP.IMU.GYROSCOPE_DATA.z);

                    /* Next step */
                    demo_step++;
                }
                /* If not success */
                else {
                    /* See if failed */
                    if (!EventManager_poll_event(
                        EVT_IMU_READ_GYRO_FAILURE, 
                        &event_raised
                    )) {
                        Debug_exit(1);
                    }

                    /* If failed print error and exit */
                    if (event_raised) {
                        DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR_CODE);
                        Debug_exit(1);
                    }
                    /* Otherwise the read just isn't finished yet, so wait a
                     * bit longer */
                }
                break;

            /* Read magne */
            case 5:
                DEBUG_INF("Issuing READ_MAGNETOMETER command");
                if (!Imu_new_command(IMU_CMD_READ_MAGNETOMETER)) {
                    Debug_exit(1);
                }
                demo_step++;
                break;

            /* Wait for read complete */
            case 6:
                /* See if success */
                if (!EventManager_poll_event(
                    EVT_IMU_READ_MAGNE_SUCCESS, 
                    &event_raised
                )) {
                    Debug_exit(1);
                }

                /* If success */
                if (event_raised) {
                    /* Print magne data */
                    DEBUG_INF("magne valid: %d", DP.IMU.MAGNETOMETER_DATA_VALID);
                    DEBUG_INF("magne x: %d", DP.IMU.MAGNETOMETER_DATA.x);
                    DEBUG_INF("magne y: %d", DP.IMU.MAGNETOMETER_DATA.y);
                    DEBUG_INF("magne z: %d", DP.IMU.MAGNETOMETER_DATA.z);

                    /* Next step */
                    demo_step++;
                }
                /* If not success */
                else {
                    /* See if failed */
                    if (!EventManager_poll_event(
                        EVT_IMU_READ_MAGNE_FAILURE, 
                        &event_raised
                    )) {
                        Debug_exit(1);
                    }

                    /* If failed print error and exit */
                    if (event_raised) {
                        DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR_CODE);
                        Debug_exit(1);
                    }
                    /* Otherwise the read just isn't finished yet, so wait a
                     * bit longer */
                }
                break;

            default:
                DEBUG_INF("End of demo reached");
                run_loop = false;
                break;
        }

        /* Step the I2C */
        i2c_error = I2c_step();
        if (i2c_error != ERROR_NONE) {
            DEBUG_ERR("I2C error code: %d", i2c_error);
            /* Don't exit as we want to see the IMU component detect this
             * error. */
        }

        /* Step the IMU */
        if (!Imu_step()) {
            DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR_CODE);
            Debug_exit(1);
        }

        #ifdef DEBUG_MODE
        if (!sleep) {
            char *p_events = NULL;
            EventManager_get_event_list_string(&p_events);
            DEBUG_INF(p_table_format, DP.IMU.STATE, DP.IMU.SUBSTATE, DP.IMU.COMMAND, DP.IMU.ERROR_CODE, p_events);
            free(p_events);
        }
        #endif

        /* Call event cleanup */
        if (!EventManager_cleanup_events()) {
            Debug_exit(1);
        }
        
        if (DP.EVENTMANAGER.NUM_RAISED_EVENTS == 0) {
            if (!sleep) {
                DEBUG_INF("No events, system sleep");
                sleep = true;
            }
        }
        else {
            if (sleep) {
                DEBUG_INF("Event, system woken");
                sleep = false;
            }
        }
    }

    /* Destroy the event manager to clear up allocated memory */
    EventManager_destroy();
}

/** @} */ /* End of demo_imu */