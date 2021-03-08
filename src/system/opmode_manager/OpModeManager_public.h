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
 * ENUMS
 * ------------------------------------------------------------------------- */

/**
 * @brief The Operational Modes (OpMode) of the spacecraft, as defined in 
 * [OPMODE].
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

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the OpModeManager.
 * 
 * @return bool True on succes, false on error. See DP.OPMODEMANAGER.ERROR_CODE for
 * cause. 
 */
bool OpModeManager_init(void);

/**
 * @brief Step the OpModeManager
 * 
 * @return bool True on succes, false on error. See DP.OPMODEMANAGER.ERROR_CODE for
 * cause. 
 */
bool OpModeManager_step(void);

#endif /* H_OPMODEMANAGER_PUBLIC_H */