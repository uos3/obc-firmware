/**
 * @defgroup imu
 *
 * @brief IMU Hardware Driver
 *
 * @details IMU Hardware Driver.
 *
 * @ingroup drivers
 */

#ifndef __IMU_H__
#define __IMU_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @function
 * @ingroup imu
 *
 * Initialise and configure IMU
 *
 */
void IMU_Init(void);

/**
 * @function
 * @ingroup imu
 *
 * Tests functionality of IMU
 *
 * @returns True if passed, False otherwise
 */
bool IMU_selftest(void);

/**
 * @function
 * @ingroup imu
 *
 * Read Accelerometer
 *
 * @parameter Pointer to x-value data storage
 * @parameter Pointer to y-value data storage
 * @parameter Pointer to z-value data storage
 */
void IMU_read_accel(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);

/**
 * @function
 * @ingroup imu
 *
 * Read Gyroscope
 *
 * @parameter Pointer to x-value data storage
 * @parameter Pointer to y-value data storage
 * @parameter Pointer to z-value data storage
 */
void IMU_read_gyro(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);

/**
 * @function
 * @ingroup imu
 *
 * Read Magnetometer
 *
 * @parameter Pointer to x-value data storage
 * @parameter Pointer to y-value data storage
 * @parameter Pointer to z-value data storage
 */
void IMU_read_magno(int16_t *magno_x, int16_t *magno_y, int16_t *magno_z);

#endif /*  __IMU_H__ */