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
    /* Init system */
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

    /* Init the IMU */
    if (!Imu_init()) {
        DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR);
        Debug_exit(1);
    }

    DEBUG_INF("IMU initialised");

    DEBUG_INF("Begining main loop\n");

    /* 
     * Demo step is a counter used to:
     * 1. Read temperature
     * 2. Read gyroscope
     * 3. Read magnetometer
     * and print the data out
     */
    uint8_t demo_step = 0;

    /* "Main loop", used to simulate the spacecraft's loop */
    bool run_loop = true;
    bool sleep = false;
    while (run_loop) {

        /* Do the demo actions */
        bool event_raised = false;
        switch (demo_step) {
            /* Issue read temp command */
            case 0:
                DEBUG_INF("Issuing READ_TEMPERATURE command");
                Imu_new_command(IMU_CMD_READ_TEMPERATURE);
                demo_step++;
                break;

            /* Wait for read complete */
            case 1:
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
                        DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR);
                        Debug_exit(1);
                    }
                    /* Otherwise the read just isn't finished yet, so wait a
                     * bit longer */
                }
                break;

            /* Issue read gyro command */
            case 2:
                DEBUG_INF("Issuing READ_GYROSCOPE command");
                Imu_new_command(IMU_CMD_READ_GYROSCOPE);
                demo_step++;
                break;
            
            /* Wait for read complete */
            case 3:
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
                        DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR);
                        Debug_exit(1);
                    }
                    /* Otherwise the read just isn't finished yet, so wait a
                     * bit longer */
                }
                break;

            /* Read magne */
            case 4:
                DEBUG_INF("Issuing READ_MAGNETOMETER command");
                Imu_new_command(IMU_CMD_READ_MAGNETOMETER);
                demo_step++;
                break;

            /* Wait for read complete */
            case 5:
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
                        DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR);
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

        /* TODO: Step I2C */

        /* Step the IMU */
        if (!Imu_step()) {
            DEBUG_ERR("IMU error code: %d", DP.IMU.ERROR);
            Debug_exit(1);
        }

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
}

/** @} */ /* End of demo_imu */