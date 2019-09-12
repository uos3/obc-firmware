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
 
#define MPU_I2C_ADDR 0x68 // this is the MPU-9250A i2c address %1011000
#define MAG_I2C_ADDR 0x0C // MAG_PASS_THROUGH_I2C_ADDR

void IMU_Init(void)
{
	uint8_t i2cstring[3]; // static buffer for i2c calls here

	i2cstring[0] = MPU_INT_BYPASS_ENABLE;
	uint8_t i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR ,MPU_INT_BYPASS_ENABLE);
	i2cstring[1] = i2cstatus | MPU_INT_BYPASS_ENABLE; // flag bypass on
	i2cstring[3] = 0; // null terminated string
	I2CcaSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring); // turn on bypass to Magnetometer so visible on I2C

	// setup magnetometer
	i2cstring[0] = MAG_CNTL1;
	i2cstring[2] = 0;
	i2cstring[1] = 0; // set mode to zero before changing mode
	I2CSendString(I2C_IMU, MAG_I2C_ADDR, i2cstring); // set Magnetometer to safe mode before mode change 
	i2cstring[1] = 0x12; // continuous (16hz) mode 1 with 16bit range
	I2CSendString(I2C_IMU, MAG_I2C_ADDR, i2cstring);
}

void IMU_set_gyro_sensitivity(imu_gyro_sensitivity_t imu_gyro_sensitivity)
{
	uint8_t i2cstatus;
	uint8_t i2cstring[3];
	switch(imu_gyro_sensitivity)
	{
		case IMU_GYRO_250DEG_S:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);		//read current configuration of config registers
			i2cstring[0] = MPU_GYRO_CONFIG;										//first byte of the packet is target register address
			i2cstring[1] = i2cstatus & 0b11100111;								//mask off bits [4:3] and keep the value of the other bits - bits [4:3] are separated by space from the others
			i2cstring[2] = 0;													//zero-valued byte means end of the packet - look definition of I2CSendString
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);					//send the packet
			break;

		case IMU_GYRO_500DEG_S:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = (i2cstatus & 0b11100111) | 0b00001000; i2cstring[2] = 0;	//for 01 need to mask in two steps - first both to 0, and then one of them to 1
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);
			break;

		case IMU_GYRO_1000DEG_S:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = (i2cstatus & 0b11100111) | 0b00010000; i2cstring[2] = 0;	//similar like for 01, but other way around
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);
			break;

		case IMU_GYRO_2000DEG_S:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus | 0b00011000; i2cstring[2] = 0;
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);
			break;

		default:	//250DEG_S
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11100111; i2cstring[2] = 0;
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);
			break;
	}
}
/* For information about the registers, values of constants look in the Register Map of the MPU9250, p.13 and 14 */
void IMU_set_gyro_bandwidth(imu_bandwidth_t imu_gyro_bandwidth)
{
	uint8_t i2cstring[3];
	uint8_t i2cstatus;
	switch(imu_gyro_bandwidth)
	{
		case IMU_BW_5HZ:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_CONFIG);												//read status of MPU_CONFIG register
			i2cstring[0] = MPU_CONFIG; i2cstring[1] = (i2cstatus | 0b00000111) & 0b11111110; i2cstring[2] = 0;		//changing bits [2:0] to 110 and keeping the value of others -> 110 refers to 5Hz
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);											//read status of MPU_GYRO_CONFIG register
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11111100; i2cstring[2] = 0;				//mask off bits [1:0] to set FCHOICE_B to 0b00 which enables larger choice of DLPF frequencies
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			break;

		case IMU_BW_10HZ:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_CONFIG);												//read status of MPU_CONFIG register
			i2cstring[0] = MPU_CONFIG; i2cstring[1] = (i2cstatus | 0b00000111) & 0b11111101; i2cstring[2] = 0;		//changing bits [2:0] to 101 and keeping the value of others -> 101 refers to 10Hz
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);											//read status of MPU_GYRO_CONFIG register
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11111100; i2cstring[2] = 0;				//mask off bits [1:0] to set FCHOICE_B to 0b00 which enables larger choice of DLPF frequencies
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			break;

		case IMU_BW_20HZ:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_CONFIG);												//read status of MPU_CONFIG register
			i2cstring[0] = MPU_CONFIG; i2cstring[1] = (i2cstatus | 0b00000111) & 0b11111100; i2cstring[2] = 0;		//changing bits [2:0] to 100 and keeping the value of others -> 100 refers to 20Hz
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);											//read status of MPU_GYRO_CONFIG register
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11111100; i2cstring[2] = 0;				//mask off bits [1:0] to set FCHOICE_B to 0b00 which enables larger choice of DLPF frequencies
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			break;

		case IMU_BW_41HZ:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_CONFIG);												//read status of MPU_CONFIG register
			i2cstring[0] = MPU_CONFIG; i2cstring[1] = (i2cstatus | 0b00000111) & 0b11111011; i2cstring[2] = 0;		//changing bits [2:0] to 011 and keeping the value of others -> 011 refers to 41Hz
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);											//read status of MPU_GYRO_CONFIG register
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11111100; i2cstring[2] = 0;				//mask off bits [1:0] to set FCHOICE_B to 0b00 which enables larger choice of DLPF frequencies
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			break;

		case IMU_BW_92HZ:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_CONFIG);												//read status of MPU_CONFIG register
			i2cstring[0] = MPU_CONFIG; i2cstring[1] = (i2cstatus | 0b00000111) & 0b11111010; i2cstring[2] = 0;		//changing bits [2:0] to 010 and keeping the value of others -> 010 refers to 92Hz
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);											//read status of MPU_GYRO_CONFIG register
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11111100; i2cstring[2] = 0;				//mask off bits [1:0] to set FCHOICE_B to 0b00 which enables larger choice of DLPF frequencies
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			break;

		case IMU_BW_184HZ:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_CONFIG);												//read status of MPU_CONFIG register
			i2cstring[0] = MPU_CONFIG; i2cstring[1] = (i2cstatus | 0b00000111) & 0b11111001; i2cstring[2] = 0;		//changing bits [2:0] to 001 and keeping the value of others -> 001 refers to 184Hz
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);											//read status of MPU_GYRO_CONFIG register
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11111100; i2cstring[2] = 0;				//mask off bits [1:0] to set FCHOICE_B to 0b00 which enables larger choice of DLPF frequencies
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			break;

		case IMU_BW_250HZ:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_CONFIG);												//read status of MPU_CONFIG register
			i2cstring[0] = MPU_CONFIG; i2cstring[1] = i2cstatus & 0b11111000; i2cstring[2] = 0;						//changing bits [2:0] to 000 and keeping the value of others -> 000 refers to 250Hz
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);											//read status of MPU_GYRO_CONFIG register
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11111100; i2cstring[2] = 0;				//mask off bits [1:0] to set FCHOICE_B to 0b00 which enables larger choice of DLPF frequencies
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			break;
		
		case IMU_BW_3600HZ:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = (i2cstatus | 0b00000011) & 0b11111110; i2cstring[2] = 0;  //changing bits [1:0] to 10 and keeping the others, to set FCHOICE_B to 10 
																													 //which cause that we don't care about the value of DLPF_CFG and are using 3600Hz 
																													 //(there is a way to use 3600Hz with FCHOICE_B = 0b00 - it will have a bit different characteristic)
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);
			break;

		case IMU_BW_8800HZ:
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR ,MPU_GYRO_CONFIG);
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11111100; i2cstring[2] = 0;				 //changing bits [1:0] to 00 to set FCHOICE_B = 00 which refers to 8800Hz
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);

		default:	//250Hz - why? -> https://github.com/betaflight/betaflight/pull/5483 
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_CONFIG);												//read status of MPU_CONFIG register
			i2cstring[0] = MPU_CONFIG; i2cstring[1] = i2cstatus & 0b11111000; i2cstring[2] = 0;					//changing bits [2:0] to 000 and keeping the value of others -> 000 refers to 250Hz
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_CONFIG);											//read status of MPU_GYRO_CONFIG register
			i2cstring[0] = MPU_GYRO_CONFIG; i2cstring[1] = i2cstatus & 0b11111100; i2cstring[2] = 0;				//mask off bits [1:0] to set FCHOICE_B to 0b00 which enables larger choice of DLPF frequencies
			I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);														//write to register
			break;
	}
}

bool IMU_selftest(void)
{
	uint8_t mpu_id = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_WHO_AM_I);	//read devices IDs to at some point indicate their functionality
	uint8_t mag_id = I2CReceive(I2C_IMU, MAG_I2C_ADDR, MAG_WIA);
	if(mpu_id == 0x71 && mag_id == 0x48)
	{
	return true;
	}
	return false;
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
	// TODO: Adjust using calibration data from ASAX, ASAY, ASAZ registers -- 

	int16_t magno_calib_x = (int16_t)I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_ASAX);
	int16_t magno_calib_y = (int16_t)I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_ASAY);
	int16_t magno_calib_z = (int16_t)I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_ASAZ);
	

	*magno_x = (int16_t)(I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_HXL) * (((magno_calib_x -128) * 0.5 ) / 128 + 1));
	*magno_y = (int16_t)(I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_HYL) * (((magno_calib_y -128) * 0.5 ) / 128 + 1));
	*magno_z = (int16_t)(I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_HZL) * (((magno_calib_z -128) * 0.5 ) / 128 + 1));
	(void)I2CReceive16r(I2C_IMU, MAG_I2C_ADDR, MAG_ST2);
}

void IMU_read_temp(int16_t *temp)
{
	*temp = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_TEMP_OUT);
}