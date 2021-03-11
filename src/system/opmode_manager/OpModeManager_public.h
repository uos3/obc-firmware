/**
 * @file OpModeManager_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief OpModeManager public header.
 * 
 * Task ref: [UT_2.9.10]
 * 
 * The OpModeManager is responsible for controlling the CubeSat's Operational
 * Mode (OpMode/OPMODE). This module is tightly tied to the OPMODE document,
 * referenced as [OPMODE].
 * 
 * References:
 * [OPMODE] - OPMODE_20200608_v2.2.vsdx
 * [CONOPS] - UoS3_CONOPS_20191113_v4.3.vsdx
 * [OPMODE_TRANSITION] - SSS_OpModeTransition_20210310_v0.1.vsdx
 * 
 * @version 0.1
 * @date 2021-03-08
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_OPMODEMANAGER_PUBLIC_H
#define H_OPMODEMANAGER_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdbool.h>

/* Internal includes */
#include "system/opmode_manager/OpModeManager_errors.h"
#include "system/opmode_manager/OpModeManager_events.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Number of OpModes available to the system
 */
#define OPMODEMANAGER_NUM_OPMODES (8)

/**
 * @brief The maximum number of Apps that can run in a mode at any one time.
 * 
 * This is used to define the DP.OPMODEMANAGER.OPMODE_APP_TABLE size.
 */
#define OPMODEMANAGER_MAX_NUM_APPS_IN_MODE (2)

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/**
 * @brief The Operational Modes (OpMode) of the spacecraft, as defined in 
 * [OPMODE].
 * 
 * NOTE: If any of these values are changed the following must also be changed:
 *  - OPMODEMANAGER_NUM_OPMODES
 *  - Power_OpModeOcpStateConfig
 */
typedef enum _OpModeManager_OpMode {

    /**
     * @brief Boot Up (BU) mode.
     */
    OPMODEMANAGER_OPMODE_BOOT_UP = 0,

    /**
     * @brief Antenna Deploy (AD) mode.
     */
    OPMODEMANAGER_OPMODE_ANTENNA_DEPLOY = 1,

    /**
     * @brief Nominal Functioning (NF) mode.
     */
    OPMODEMANAGER_OPMODE_NOMINAL_FUNCTIONING = 2,

    /**
     * @brief Low Power (LP) mode.
     */
    OPMODEMANAGER_OPMODE_LOW_POWER = 3,

    /**
     * @brief Safe (S) mode.
     */
    OPMODEMANAGER_OPMODE_SAFE = 4,

    /**
     * @brief Data Downlink (DL) mode.
     * 
     * TODO: DL should be removed and made a part of the general comms system,
     * but with support for critical telemetry downlinking in S/LP.
     */
    OPMODEMANAGER_OPMODE_DATA_DOWNLINK = 5,

    /**
     * @brief Configuration File Update (CFU) mode.
     */
    OPMODEMANAGER_OPMODE_CONFIG_FILE_UPDATE = 6,

    /**
     * @brief Picture Taking (PT) mode.
     */
    OPMODEMANAGER_OPMODE_PICTURE_TAKING = 7

} OpModeManager_OpMode;

/**
 * @brief The state of the OpModeManager. Not to be confused with the OpMode
 * itself.
 * 
 * The OpModeManager state machine is decoupled from the OpModes themselves in
 * order to:
 *  - Clearly separate OpMode transition logic from the actual mode being
 *    entered 
 *  - Allow the OpModeManager to be in one of two different general types of
 *    transition - graceful and emergency.
 *  - Allow for easy addition of new OpModes, making the system generic over
 *    the mission.
 */
typedef enum _OpModeManager_State {
    /**
     * @brief The OpModeManager is Off, this is only the case before init of
     * the module.
     */
    OPMODEMANAGER_STATE_OFF = 0,

    /**
     * @brief The OpModeManager is executing an OpMode. This is the normal
     * state of the manager.
     */
    OPMODEMANAGER_STATE_EXECUTING,

    /**
     * @brief Graceful transition state, which proceeds as described in
     * [OPMODE_TRANSITION]. 
     */
    OPMODEMANAGER_STATE_GRACEFUL_TRANSITION,

    /**
     * @brief Emergency transition state, which proceeds as described in
     * [OPMODE_TRANSITION]. 
     */
    OPMODEMANAGER_STATE_EMERGENCY_TRANSITION
} OpModeManager_State;

/**
 * @brief Represents the state of a graceful transition.
 * 
 * See [OPMODE_TRANSITION].
 */
typedef enum _OpModeManager_GraceTransState {
    /**
     * @brief Initiate graceful transition state, in which the active apps are
     * instructed to be OFF, unless they are also active in the next mode.
     */
    OPMODEMANAGER_GRACETRANSSTATE_INIT = 0,

    /**
     * @brief Wait for all active apps to be off state. This state can timeout,
     * in which case active apps will be forced off.
     */
    OPMODEMANAGER_GRACETRANSSTATE_WAIT_ACTIVE_OFF,

    /**
     * @brief Wait for Power to signal successful OCP state change state.
     */
    OPMODEMANAGER_GRACETRANSSTATE_WAIT_OCP_CHANGE,
} OpModeManager_GraceTransState;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the OpModeManager.
 * 
 * @return bool True on succes, false on error. See DP.OPMODEMANAGER.ERROR_CODE
 * for cause. 
 */
bool OpModeManager_init(void);

/**
 * @brief Step the OpModeManager
 * 
 * @return bool True on succes, false on error. See DP.OPMODEMANAGER.ERROR_CODE
 * for cause. 
 */
bool OpModeManager_step(void);

#endif /* H_OPMODEMANAGER_PUBLIC_H */