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
 
#define SLAVE_ADDRESS 0x68 // this is the MPU-9250A i2c address %1011000

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

#define MAG_PASS_THROUGH_I2C_ADDR 12 

#define MAG_HXL 3
#define MAG_HYL 5
#define MAG_HZL 7 // offsets in magnetometer, note little endian, so need to be read opposite way round
#define MAG_CNTL1 10 // Magnetometer control register
#define MAG_STA1 2 // read only gives flags at 1 for DRDY and 2 for DOR (data ready and overload), used in read cycle

void IMU_Init(void)
{
	// enable magnetometer to appear separately I2C by commanding MPU chip
	uint8_t i2cstring[3]; // static buffer for i2c calls here
	i2cstring[0]=MPU_INT_BYPASS_ENABLE; // int pin /bypass enable configuration
	uint8_t i2cstatus=I2CReceive(I2C_IMU, SLAVE_ADDRESS,MPU_INT_BYPASS_ENABLE);
	i2cstring[1]=i2cstatus | MPU_INT_BYPASS_ENABLE; // flag bypass on
	i2cstring[3]=0; // null terminated string
	I2CSendString(I2C_IMU, SLAVE_ADDRESS, i2cstring); // turn on bypass to Magnetometer so visible on I2C

	// setup magnetometer
	i2cstring[0]=MAG_CNTL1;
	i2cstring[2]=0;
	i2cstring[1]=0; // set mode to zero before changing mode
	I2CSendString(I2C_IMU, MAG_PASS_THROUGH_I2C_ADDR, i2cstring); // set Magnetometer to safe mode before mode change 
	i2cstring[1]=1 | 0; // continuous (16hz) mode with 14bit range
	I2CSendString(I2C_IMU, MAG_PASS_THROUGH_I2C_ADDR, i2cstring); // set Magnetometer to continuous shot mode with 16bit range, single shot needs updating and has delay 
}

bool IMU_selftest(void)
{
	//uint8_t mag_id=I2CReceive(I2C_IMU, MAG_PASS_THROUGH_I2C_ADDR,0); // magnetometer ID (hopefully)

	return true;
}

void IMU_read_accel(uint16_t *accel_x, uint16_t *accel_y, uint16_t *accel_z)
{
	*accel_x = I2CReceive16(I2C_IMU, SLAVE_ADDRESS, MPU_ACCEL_XOUT);
	*accel_y = I2CReceive16(I2C_IMU, SLAVE_ADDRESS, MPU_ACCEL_YOUT);
	*accel_z = I2CReceive16(I2C_IMU, SLAVE_ADDRESS, MPU_ACCEL_ZOUT);
}

void IMU_read_gyro(uint16_t *gyro_x, uint16_t *gyro_y, uint16_t *gyro_z)
{
	*gyro_x = I2CReceive16(I2C_IMU, SLAVE_ADDRESS, MPU_GYRO_XOUT);
	*gyro_y = I2CReceive16(I2C_IMU, SLAVE_ADDRESS, MPU_GYRO_YOUT);
	*gyro_z = I2CReceive16(I2C_IMU, SLAVE_ADDRESS, MPU_GYRO_ZOUT);
}

void IMU_read_magno(uint16_t *magno_x, uint16_t *magno_y, uint16_t *magno_z)
{
	*magno_x = I2CReceive16r(I2C_IMU, MAG_PASS_THROUGH_I2C_ADDR, MAG_HXL);
	*magno_y = I2CReceive16r(I2C_IMU, MAG_PASS_THROUGH_I2C_ADDR, MAG_HYL);
	*magno_z = I2CReceive16r(I2C_IMU, MAG_PASS_THROUGH_I2C_ADDR, MAG_HZL);
}
