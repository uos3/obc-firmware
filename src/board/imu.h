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

// MPU 9250 registers
#define MPU_GYRO_X_OFFS_H		19		//value of the x-axis offset - high byte (low byte in register 20)
#define MPU_GYRO_Y_OFFS_H		21 		//value of the y-axis offset - high byte (low byte in register 22)
#define MPU_GYRO_Z_OFFS_H		23		//value of the z-axis offset - high byte (low byte in register 24)
#define MPU_CONFIG 				26		//configuration register of MPU - contains bits used for the settings of DLPF (digital low pass filter)
#define MPU_GYRO_CONFIG 		27		//gyroscope configuration register for configuring gyro full scale
#define MPU_ACCEL_CONFIG 		28		//accelerometer configuration register for configuring accel full scale
#define MPU_ACCEL_XOUT 			59		
#define MPU_ACCEL_YOUT 			61
#define MPU_ACCEL_ZOUT 			63
#define MPU_TEMP_OUT 			65
#define MPU_GYRO_XOUT 			67
#define MPU_GYRO_YOUT 			69
#define MPU_GYRO_ZOUT 			71
#define MPU_WHO_AM_I 			117
#define MPU_INT_BYPASS_ENABLE 	55

#define MAG_WIA 0	//contains device ID
#define MAG_STA1 2 	//read only gives flags at 1 for DRDY and 2 for DOR (data ready and overload), used in read cycle
#define MAG_HXL 3
#define MAG_HYL 5
#define MAG_HZL 7 	//offsets in magnetometer, note little endian, so need to be read opposite way round
#define MAG_ST2 9
#define MAG_CNTL1 10 // Magnetometer control register
#define MAG_ASAX 16
#define MAG_ASAY 17
#define MAG_ASAZ 18

typedef enum
{
	IMU_BW_5HZ,
	IMU_BW_10HZ,
	IMU_BW_20HZ,
	IMU_BW_41HZ,
	IMU_BW_92HZ,
	IMU_BW_184HZ,
	IMU_BW_250HZ,
	IMU_BW_3600HZ
} imu_gyro_bandwidth_t;

/* Integer values of those settings are defined in the Register Map document p.14 -> Register 27, bits [4:3], not used as values, but written for reference */
typedef enum
{
	IMU_GYRO_250DEG_S = 0b00,		
	IMU_GYRO_500DEG_S = 0b01,		
	IMU_GYRO_1000DEG_S = 0b10,		
	IMU_GYRO_2000DEG_S = 0b11		
} imu_gyro_range_t;
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
 * Read Gyroscope Raw values
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
 * Read IMU Temperature
 *
 * @parameter Pointer to temperature-value data storage
 */

void IMU_read_temp(int16_t *temp);

/**
 * @function
 * @ingroup imu
 * 
 * Calibrate gyroscope of the IMU chip by removing DC offsets
 * 
 * @parameter Pointer to gx_offset data storage
 * @parameter Pointer to gy_offset data storage
 * @parameter Pointer to gz_offset data storage
 */
void IMU_calibrate_gyro(int16_t *new_gx_offset, int16_t *new_gy_offset, int16_t *new_gz_offset);

/**
 * @function
 * @ingroup imu
 * 
 * Adjust the sensitivity and range of the gyro measurements - default range +/- 250 dps
 * 
 * @parameter Enumerated setting of the gyro measurement range
 */
void IMU_set_gyro_sensitivity(imu_gyro_range_t imu_gyro_range);

/**
 * @function
 * @ingroup imu
 * 
 * Adjust the bandwidth of the gyroscope: changes cut off frequency of DLPF and rate of the measurements to provide noiseless results
 * Check for more information: https://github.com/betaflight/betaflight/pull/5483 and google "MPU9250 dlpf settings"
 * 
 * @parameter Enumerated setting of the gyro bandwidth
 */
void IMU_set_gyro_bandwidth(imu_gyro_bandwidth_t imu_gyro_bandwidth);



#endif /*  __IMU_H__ */