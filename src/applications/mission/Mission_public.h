/**
 * @file Mission_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Mission Application public header.
 * 
 * Task ref: [UT_2.11.1]
 * 
 * The Mission app is responsible for managing the overall high-level mission
 * behaviour of the system. This includes:
 *  - Operational Mode (OpMode) management
 * 
 * TODO: This is a temporary working version providing OpModes for PowerApp
 * development.
 * 
 * References:
 *  - [OPMODE] - OPMODE_20200608_v2.2.vsdx - Operational Modes Diagram
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_MISSION_PUBLIC_H
#define H_MISSION_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdbool.h>

/* Internal includes */
#include "applications/mission/Mission_errors.h"
#include "applications/mission/Mission_events.h"

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/**
 * @brief The Operational Modes (OpMode) of the spacecraft, as defined in 
 * [OPMODE].
 */
typedef enum _Mission_OpMode {

    /**
     * @brief Boot Up (BU) mode.
     */
    MISSION_OPMODE_BOOT_UP = 0,

    /**
     * @brief Antenna Deploy (AD) mode.
     */
    MISSION_OPMODE_ANTENNA_DEPLOY = 1,

    /**
     * @brief Nominal Functioning (NF) mode.
     */
    MISSION_OPMODE_NOMINAL_FUNCTIONING = 2,

    /**
     * @brief Low Power (LP) mode.
     */
    MISSION_OPMODE_LOW_POWER = 3,

    /**
     * @brief Safe (S) mode.
     */
    MISSION_OPMODE_SAFE = 4,

    /**
     * @brief Data Downlink (DL) mode.
     */
    MISSION_OPMODE_DATA_DOWNLINK = 5,

    /**
     * @brief Configuration File Update (CFU) mode.
     */
    MISSION_OPMODE_CONFIG_FILE_UPDATE = 6,

    /**
     * @brief Picture Taking (PT) mode.
     */
    MISSION_OPMODE_PICTURE_TAKING = 7

} Mission_OpMode;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the main Mission application.
 * 
 * @return bool True on succes, false on error. See DP.MISSION.ERROR_CODE for
 * cause. 
 */
bool Mission_init(void);

/**
 * @brief Step the Mission app
 * 
 * @return bool True on succes, false on error. See DP.MISSION.ERROR_CODE for
 * cause. 
 */
bool Mission_step(void);

#ifdef DEBUG_MODE
/**
 * @brief Begin a change in OPMODE.
 * 
 * Note: This function is for development testing only.
 * 
 * @param opmode_in The OPMODE to change to
 * @return bool True on succes, false on error. See DP.MISSION.ERROR_CODE for
 * cause. 
 */
bool Mission_start_opmode_change(Mission_OpMode opmode_in);
#endif

#endif /* H_MISSION_PUBLIC_H */