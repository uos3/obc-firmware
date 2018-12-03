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

typedef enum
{
	IMU_BW_5HZ,
	IMU_BW_10HZ,
	IMU_BW_20HZ,
	IMU_BW_41HZ,
	IMU_BW_92HZ,
	IMU_BW_184HZ,
	IMU_BW_460HZ
} imu_bandwidth_t;

typedef enum
{
	IMU_ACCEL_2G,
	IMU_ACCEL_4G,
	IMU_ACCEL_8G,
	IMU_ACCEL_16G
} imu_accel_sensitivity_t;

typedef enum
{
	IMU_GYRO_250DEG_S,
	IMU_GYRO_500DEG_S,
	IMU_GYRO_1000DEG_S,
	IMU_GYRO_2000DEG_S
} imu_gyro_sensitivity_t;

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

/**
 * @function
 * @ingroup imu
 *
 * Read temperature from IMU
 *
 * @parameter Pointer to temperature data storage
 */
void IMU_read_temp(int16_t *temp_imu);

void IMU_set_accel_sensitivity(imu_accel_sensitivity_t imu_accel_sensitivity);
void IMU_set_accel_bandwidth(imu_bandwidth_t imu_accel_bandwidth);

void IMU_set_gyro_sensitivity(imu_gyro_sensitivity_t imu_gyro_sensitivity);
void IMU_set_gyro_bandwidth(imu_bandwidth_t imu_gyro_bandwidth);



#endif /*  __IMU_H__ */
