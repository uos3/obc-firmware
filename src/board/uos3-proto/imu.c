/**
 * @ingroup uos3-proto
 * @ingroup imu
 *
 * @file uos3-proto/imu.c
 * @brief IMU Hardware Driver - uos3-proto board
 *
 * @{
 */

#include "board.h"
#include "../i2c.h"

#include "../imu.h"

 // the useful hardware constants (only used here so not moved away as yet)
 
#define MPU_I2C_ADDR 0x68 // this is the MPU-9250A i2c address %1011000
#define MAG_I2C_ADDR 0x0C // MAG_PASS_THROUGH_I2C_ADDR

// MPU 9250 registers 

#define MPU_CONFIG 26
#define MPU_GYRO_CONFIG 27
#define MPU_ACCEL_CONFIG 28
#define MPU_ACCEL_XOUT 59
#define MPU_ACCEL_YOUT 61
#define MPU_ACCEL_ZOUT 63
#define MPU_TEMP_OUT 65
#define MPU_GYRO_XOUT 67
#define MPU_GYRO_YOUT 69
#define MPU_GYRO_ZOUT 71
#define MPU_WHO_AM_I 117
#define MPU_INT_BYPASS_ENABLE 55

#define MAG_HXL 3
#define MAG_HYL 5
#define MAG_HZL 7 // offsets in magnetometer, note little endian, so need to be read opposite way round
#define MAG_ST2 0x09
#define MAG_CNTL1 10 // Magnetometer control register
#define MAG_STA1 2 // read only gives flags at 1 for DRDY and 2 for DOR (data ready and overload), used in read cycle

void IMU_Init(void)
{
	uint8_t i2cstring[3]; // static buffer for i2c calls here

	i2cstring[0] = MPU_INT_BYPASS_ENABLE;
	uint8_t i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR ,MPU_INT_BYPASS_ENABLE);
	i2cstring[1] = i2cstatus | MPU_INT_BYPASS_ENABLE; // flag bypass on
	i2cstring[3] = 0; // null terminated string
	I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring); // turn on bypass to Magnetometer so visible on I2C

	// setup magnetometer
	i2cstring[0] = MAG_CNTL1;
	i2cstring[2] = 0;
	i2cstring[1] = 0; // set mode to zero before changing mode
	I2CSendString(I2C_IMU, MAG_I2C_ADDR, i2cstring); // set Magnetometer to safe mode before mode change 
	i2cstring[1] = 0x12; // continuous (16hz) mode 1 with 16bit range
	I2CSendString(I2C_IMU, MAG_I2C_ADDR, i2cstring);
}

void IMU_set_accel_sensitivity(imu_accel_sensitivity_t imu_accel_sensitivity)
{
	switch(imu_accel_sensitivity)
	{
		case IMU_ACCEL_2G:
			break;

		case IMU_ACCEL_4G:
			break;

		case IMU_ACCEL_8G:
			break;

		case IMU_ACCEL_16G:
			break;

		default:
			break;
	}
}

void IMU_set_accel_bandwidth(imu_bandwidth_t imu_accel_bandwidth)
{
	switch(imu_accel_bandwidth)
	{
		case IMU_BW_5HZ:
			break;

		case IMU_BW_10HZ:
			break;

		case IMU_BW_20HZ:
			break;

		case IMU_BW_41HZ:
			break;

		case IMU_BW_92HZ:
			break;

		case IMU_BW_184HZ:
			break;

		case IMU_BW_460HZ:
			break;

		default:
			break;
	}
}

void IMU_set_gyro_sensitivity(imu_gyro_sensitivity_t imu_gyro_sensitivity)
{
	switch(imu_gyro_sensitivity)
	{
		case IMU_GYRO_250DEG_S:
			break;

		case IMU_GYRO_500DEG_S:
			break;

		case IMU_GYRO_1000DEG_S:
			break;

		case IMU_GYRO_2000DEG_S:
			break;

		default:
			break;
	}
}

void IMU_set_gyro_bandwidth(imu_bandwidth_t imu_gyro_bandwidth)
{
	switch(imu_gyro_bandwidth)
	{
		case IMU_BW_5HZ:
			break;

		case IMU_BW_10HZ:
			break;

		case IMU_BW_20HZ:
			break;

		case IMU_BW_41HZ:
			break;

		case IMU_BW_92HZ:
			break;

		case IMU_BW_184HZ:
			break;

		case IMU_BW_460HZ:
			break;

		default:
			break;
	}
}

bool IMU_selftest(void)
{
	//uint8_t mag_id=I2CReceive(I2C_IMU, MAG_PASS_THROUGH_I2C_ADDR,0); // magnetometer ID (hopefully)

	return true;
}

void IMU_read_accel(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z)
{
	*accel_x = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_ACCEL_XOUT);
	*accel_y = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_ACCEL_YOUT);
	*accel_z = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_ACCEL_ZOUT);
}

void IMU_read_gyro(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z)
{
	// TODO: DC Bias must be removed by calibrating regs 19-24

	*gyro_x = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_XOUT);
	*gyro_y = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_YOUT);
	*gyro_z = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_ZOUT);
}

void IMU_read_magno(int16_t *magno_x, int16_t *magno_y, int16_t *magno_z)
{
	// TODO: Adjust using calibration data from ASAX, ASAY, ASAZ registers.

	*magno_x = (int16_t)I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_HXL);
	*magno_y = (int16_t)I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_HYL);
	*magno_z = (int16_t)I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_HZL);
	(void)I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_ST2);
}

void IMU_read_temp(int16_t *temp_imu)
{
  // TODO: Add adjustments based on calibration data in register reference.
  // It is currently unclear where to get these values, awaiting reply from the manufacturer before implementing.
  *temp_imu = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_TEMP_OUT);
  //*temp_imu = (int16_t)((*temp_imu - RoomTemp_Offset)/Temp_Sensitivity + 21);
}
