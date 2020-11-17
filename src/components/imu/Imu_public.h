/**
 * @ingroup component
 * @file Imu_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief IMU component module.
 * 
 * Task ref: [UT_2.10.1]
 * 
 * This module provides an interface to manage and use the onboard IMU (an 
 * MPU-9250A).
 * 
 * Two main reference documents are used in this module:
 *  - [PRODSPEC] - MPU-9250 Product Specification v1.1
 *  - [REGMAP] - MPU-9250 Register Map and Descriptions v1.6
 * 
 * Calibration tooling is provided so that the IMU can be easily calibrated
 * using the `tools/tool_imu_calibrate` executable. Including the calibration
 * code is enabled by passing the `imu_calib` flag to the build script, which
 * enables the 
 * 
 * @version 0.1
 * @date 2020-11-17
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup imu IMU
 * @{
 */

#ifndef H_IMU_PUBLIC_H
#define H_IMU_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#ifdef F_IMU_CALIB
#include "components/imu/Imu_calib.h"
#endif

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the IMU component.
 * 
 * This function will initialise the hardware required to use the IMU 
 * component.
 * 
 * @return bool True on success, false on error.
 */
bool Imu_init(void);

#endif /* H_IMU_PUBLIC_H */

/** @} */ /* End of IMU group */