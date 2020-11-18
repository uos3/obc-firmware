/**
 * @ingroup imu
 * @file Imu_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief IMU component public source, see corresponding header file for more 
 * information.
 * 
 * Task ref: [UT_2.10.1]
 * 
 * @version 0.1
 * @date 2020-11-17
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdbool.h>

/* Internal includes */
#include "system/data_pool/DataPool_public.h"
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "components/imu/Imu_public.h"
#include "components/imu/Imu_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Imu_init(void) {
    /* Check DP, and EM are initialised */
    if (!DP.INITIALISED) {
        DEBUG_ERR("Cannot use Imu as DataPool is not initialised.");
        return false;
    }

    if (!DP.EVENTMANAGER.INITIALISED) {
        DEBUG_ERR("Cannot use Imu as EventManager is not initialised.");
        return false;
    }

    // TODO

    /* Set the initialised flag in the DP */
    DP.IMU.INITIALISED = true;

    /* TODO: should we actually do the self-test first rather than set the
     * offsets? As fas as I can tell from [SELFTEST] the offsets do not need to
     * be set to use self-test since it's based on raw factory readings. */

    /* Call the begin function for the first state */
    if (!Imu_begin_set_gyro_offsets()) {
        /* Do not set DP.IMU.ERROR as Imu_begin_set_gyro_offsets will do this*/
        return false;
    }

    return true;
}

bool Imu_step(void) {
    /* Check the IMU is initialised */
    if (!DP.IMU.INITIALISED) {
        DEBUG_ERR("Attempted to step Imu when not initialised");
        DP.IMU.ERROR = IMU_ERROR_NOT_INITIALISED;
        return false;
    }

    /* Run the main state machine */
    switch (DP.IMU.STATE) {
        case IMU_STATE_SET_GYROSCOPE_OFFSETS: ;

            /* Step the state itself */
            if (!Imu_step_set_gyro_offsets()) {
                /* Message and error code set by the step function */
                return false;
            }

            break;
        case IMU_STATE_SELF_TEST: ;

            /* Step the state itself */
            if (!Imu_step_self_test()) {
                /* Message and error code set by the step function */
                return false;
            }

            break;
        case IMU_STATE_WAIT_NEW_COMMAND: ;

            /* This state is simple and so is implemented here rather than in a
             * separate function. There is no transition out of this state
             * except if a valid new command is found. */

            /* Poll for new event */
            bool new_command = false;
            if (!EventManager_poll_event(EVT_IMU_NEW_COMMAND, &new_command)) {
                DEBUG_ERR("EventManager error occured");
                DP.IMU.ERROR = IMU_ERROR_EVENTMANAGER_ERROR;
                return false;
            }

            /* If we got a new command switch into that command's corresponding
             * state. */
            if (new_command) {
                switch (DP.IMU.COMMAND) {
                    case IMU_CMD_NONE:
                        DEBUG_ERR(
                            "New command event raised when no command set in DP"
                        );
                        DP.IMU.ERROR = IMU_ERROR_NEW_NONE_COMMAND;
                        return false;
                    
                    case IMU_CMD_READ_GYROSCOPE:

                        /* Begin the gyro state */
                        if (!Imu_begin_read_gyro()) {
                            /* Error message and code set by the begin func */
                            return false;
                        }

                        break;
                    case IMU_CMD_READ_MAGNETOMETER:
                        
                        /* Begin the magne state */
                        if (!Imu_begin_read_magne()) {
                            /* Error message and code set by the begin func */
                            return false;
                        }

                        break;
                    case IMU_CMD_READ_TEMPERATURE:
                        
                        /* Begin the gyro state */
                        if (!Imu_begin_read_temp()) {
                            /* Error message and code set by the begin func */
                            return false;
                        }

                        break;
                    default:
                        DEBUG_ERR("Unrecognised command");
                        DP.IMU.ERROR = IMU_ERROR_UNRECOGNISED_CMD;
                        return false;
                }
            }

            break;
        case IMU_STATE_READ_TEMPERATURE:

            /* Step the state itself */
            if (!Imu_step_read_temp()) {
                /* Message and error code set by the step function */
                return false;
            }

            break;
        case IMU_STATE_READ_GYROSCOPE:

            /* Step the state itself */
            if (!Imu_step_read_gyro()) {
                /* Message and error code set by the step function */
                return false;
            }

            break;
        case IMU_STATE_READ_MAGNETOMETER:

            /* Step the state itself */
            if (!Imu_step_read_magne()) {
                /* Message and error code set by the step function */
                return false;
            }

            break;
        default:
            DEBUG_ERR("Unrecognised DP.IMU.STATE");
            DP.IMU.ERROR = IMU_ERROR_UNRECOGNISED_STATE;
            return false;
    }

    return true;
}

bool Imu_new_command(Imu_Command command_in) {
    /* Set the command in the datapool and then raise the new command event */
    DP.IMU.COMMAND = command_in;

    return EventManager_raise_event(EVT_IMU_NEW_COMMAND);
}