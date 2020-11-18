/**
 * @ingroup imu
 * @file Imu_private.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief IMU component private source, see corresponding header file for more 
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

/* Internal includes */
#include "system/data_pool/DataPool_public.h"
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "components/imu/Imu_public.h"
#include "components/imu/Imu_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/* TODO: These begin functions may actually be easier as a single function, but
 * I'm not going to do that yet as there may be a need to do something more
 * complex in a begin function that I haven't found yet. */

bool Imu_begin_set_gyro_offsets(void) {
    /* Set the state and substate to the correct values */
    DP.IMU.STATE = IMU_STATE_SET_GYROSCOPE_OFFSETS;
    DP.IMU.SUBSTATE = IMU_SUBSTATE_SET_GYROSCOPE_X_OFFSET;

    /* Raise the state change event, which will ensure that the system does not
     * go to sleep and immediately executes the next cycle */
    return EventManager_raise_event(EVT_IMU_STATE_CHANGE);
}

bool Imu_begin_self_test(void) {
    /* Set the state and substate to the correct values */
    DP.IMU.STATE = IMU_STATE_SELF_TEST;
    DP.IMU.SUBSTATE = IMU_SUBSTATE_NONE;

    /* Raise the state change event, which will ensure that the system does not
     * go to sleep and immediately executes the next cycle */
    return EventManager_raise_event(EVT_IMU_STATE_CHANGE);
}

bool Imu_begin_wait_new_command(void) {
    /* Set the state and substate to the correct values */
    DP.IMU.STATE = IMU_STATE_WAIT_NEW_COMMAND;
    DP.IMU.SUBSTATE = IMU_SUBSTATE_NONE;

    /* Raise the state change event, which will ensure that the system does not
     * go to sleep and immediately executes the next cycle */
    return EventManager_raise_event(EVT_IMU_STATE_CHANGE);
}

bool Imu_begin_read_temp(void) {
    /* Set the state and substate to the correct values */
    DP.IMU.STATE = IMU_STATE_READ_TEMPERATURE;
    DP.IMU.SUBSTATE = IMU_SUBSTATE_NONE;

    /* Raise the state change event, which will ensure that the system does not
     * go to sleep and immediately executes the next cycle */
    return EventManager_raise_event(EVT_IMU_STATE_CHANGE);
}

bool Imu_begin_read_gyro(void) {
    /* Set the state and substate to the correct values */
    DP.IMU.STATE = IMU_STATE_READ_GYROSCOPE;
    DP.IMU.SUBSTATE = IMU_SUBSTATE_READ_GYROSCOPE_X;

    /* Raise the state change event, which will ensure that the system does not
     * go to sleep and immediately executes the next cycle */
    return EventManager_raise_event(EVT_IMU_STATE_CHANGE);
}

bool Imu_begin_read_magne(void) {
    /* Set the state and substate to the correct values */
    DP.IMU.STATE = IMU_STATE_READ_MAGNETOMETER;
    DP.IMU.SUBSTATE = IMU_SUBSTATE_READ_MAGNE_SENSE_ADJUST_X;

    /* Raise the state change event, which will ensure that the system does not
     * go to sleep and immediately executes the next cycle */
    return EventManager_raise_event(EVT_IMU_STATE_CHANGE);
}

bool Imu_step_set_gyro_offsets(void) {
    // TODO
    return true;
}

bool Imu_step_self_test(void) {
    // TODO
    return true;
}

bool Imu_step_read_temp(void) {
    // TODO
    return true;
}

bool Imu_step_read_gyro(void) {
    // TODO
    return true;
}

bool Imu_step_read_magne(void) {
    // TODO
    return true;
}