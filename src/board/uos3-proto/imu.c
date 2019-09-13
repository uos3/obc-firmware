/**
 * @ingroup uos3-proto
 * @ingroup imu
 *
 * @file uos3-proto/imu.c
 * @brief IMU Hardware Driver - uos3-proto board
 *
 * @{
 */
/* Does the calibration of the gyro and magnetometer to the field here, in Southampton have a sense? in terms of it going to the space?
	the code for calibration of the gyroscope offsets is already written; the idea and example of the magnetometer calibration can be found
	here: https://github.com/kriswiner/MPU6050/wiki/Simple-and-Effective-Magnetometer-Calibration;
	gyro calibration code was written basing on this source: https://wired.chillibasket.com/2015/01/calibrating-mpu6050/ 
	Notes for self-test of imu: https://github.com/kriswiner/MPU9250/tree/master/Documents */
#include "board.h"
#include "../i2c.h"
#include "../delay.h"
#include "../imu.h"

#define MPU_I2C_ADDR 0x68 // this is the MPU-9250A i2c address %1011000
#define MAG_I2C_ADDR 0x0C // MAG_PASS_THROUGH_I2C_ADDR

void IMU_Init(void)
{
	uint8_t i2cstring[3]; // static buffer for i2c calls here

	i2cstring[0] = MPU_INT_BYPASS_ENABLE;
	uint8_t i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR ,MPU_INT_BYPASS_ENABLE);
	i2cstring[1] = i2cstatus | MPU_INT_BYPASS_ENABLE; // flag bypass on
	i2cstring[2] = 0; // null terminated string
	I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring); // turn on bypass to Magnetometer so visible on I2C

	// setup magnetometer
	i2cstring[0] = MAG_CNTL1;
	i2cstring[2] = 0;
	i2cstring[1] = 0; // set mode to zero before changing mode
	I2CSendString(I2C_IMU, MAG_I2C_ADDR, i2cstring); // set Magnetometer to safe mode before mode change 
	i2cstring[1] = 0x12; // continuous (16hz) mode 1 with 16bit range
	I2CSendString(I2C_IMU, MAG_I2C_ADDR, i2cstring);

	/* Setup gyro sensitivity */
	IMU_set_gyro_sensitivity(IMU_GYRO_250DEG_S);	//the lowest range giving the highest sensitivity - that's why chosen as default
	IMU_set_gyro_bandwidth(IMU_BW_92HZ);			//chosen because given in the 
}

void IMU_set_gyro_sensitivity(imu_gyro_range_t imu_gyro_range)
{
	uint8_t i2cstatus;
	uint8_t i2cstring[3];
	switch(imu_gyro_range)
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
/* Going for lower cut off frequency of DLPF will give less error but as a trade off the delay between samples will be bigger */
/* It is not significant fot our application as we took readings with the 500ms delay between them - so by future tests the most reliable BW can be established - probably one of the lowest - 13/09/2019 Maciej */
void IMU_set_gyro_bandwidth(imu_gyro_bandwidth_t imu_gyro_bandwidth)
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

		default:	//92Hz because it gives 0.1 noise - refer to datasheet
			i2cstatus = I2CReceive(I2C_IMU, MPU_I2C_ADDR, MPU_CONFIG);												//read status of MPU_CONFIG register
			i2cstring[0] = MPU_CONFIG; i2cstring[1] = (i2cstatus | 0b00000111) & 0b11111010; i2cstring[2] = 0;		//changing bits [2:0] to 010 and keeping the value of others -> 000 refers to 250Hz
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
	/* DC Bias must be removed by calibrating regs 19-24 -> calibration function*/

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

/* This function can be used only when the board/satellite is on the flat surface, not moving - it removes the DC offset in readings basing on the average of multiple readings */
/* Will be used in the IMU demo code - assuming that test is done with static (not moving) satellite and in "calibrate_imu.c" script which can be used before the mission and dynamic tests */
void IMU_calibrate_gyro(int16_t *new_gx_offset, int16_t *new_gy_offset, int16_t *new_gz_offset){
	int8_t gyro_calibration_precision = 15;
	int16_t gyro_x, gyro_y, gyro_z;							//variables to store readings
	int16_t mean_gx = 0, mean_gy = 0, mean_gz = 0;			//variables to store the mean of all the readings
	int16_t gx_offset, gy_offset, gz_offset;
	uint8_t i2cstring[4];
	int i = 0;												//variable for the while loop
char output[200];
	/* Take measurements */
	IMU_measumerements_for_calib(&mean_gx, &mean_gy, &mean_gz);
	Delay_ms(1000);
	/* Calculate new offsets */
	gx_offset = -mean_gx/4;			//division by 4 taken from the Register Map Document, unit of all variables is LSB
	gy_offset = -mean_gy/4;
	gz_offset = -mean_gz/4;
	/* Loop to perform the calibration until required precision is achieved */
	while(1){
		int ready = 0;
		/* Load new offsets to the device - using burst write - slave is starting from the given register and ascending*/
		/* First byte of the string - address of first register, second byte - top 8 bits of the variable, third byte - 8 lower bits of the variable */
		/* x - axis */
		i2cstring[0] = MPU_GYRO_X_OFFS_H; i2cstring[1] = (uint8_t) (gx_offset >> 8); i2cstring[2] = (uint8_t) gx_offset; i2cstring[3] = 0;
		I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);
		/* y - axis */
		i2cstring[0] = MPU_GYRO_Y_OFFS_H; i2cstring[1] = (uint8_t) (gy_offset >> 8); i2cstring[2] = (uint8_t) gy_offset; i2cstring[3] = 0;
		I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);
		/* z - axis */
		i2cstring[0] = MPU_GYRO_Z_OFFS_H; i2cstring[1] = (uint8_t) (gz_offset >> 8); i2cstring[2] = (uint8_t) gz_offset; i2cstring[3] = 0;
		I2CSendString(I2C_IMU, MPU_I2C_ADDR, i2cstring);

		/* Read from sensors again to check new measurements */
		IMU_measumerements_for_calib(&mean_gx, &mean_gy, &mean_gz);
		if(-gyro_calibration_precision<mean_gx && mean_gx<gyro_calibration_precision) ready++;	//if mean of the readings fits in between the <-1;1> range signal that it is ready
		else gx_offset = gx_offset - mean_gx/(gyro_calibration_precision+1);					//if not adjust the offset slightly (+1 is empirical)

		if(-gyro_calibration_precision<mean_gy && mean_gy<gyro_calibration_precision) ready++;
		else gy_offset = gy_offset - mean_gy/(gyro_calibration_precision+1);

		if(-gyro_calibration_precision<mean_gz && mean_gz<gyro_calibration_precision) ready++;
		else gz_offset = gz_offset - mean_gz/(gyro_calibration_precision+1);

		if(ready == 3) break;	//when all three axis are calibrated break the loop
		UART_puts(UART_DEBUG_4, "Recalibration\r\n");
		sprintf(output, ">>> Gyro Offsets: X %+06d, Y %+06d, Z %+06d\r\n", gx_offset,gy_offset, gz_offset);
    	UART_puts(UART_DEBUG_4, output);

	}
	/* Read current offset values */
	*new_gx_offset = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_X_OFFS_H);
	*new_gy_offset = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_Y_OFFS_H);
	*new_gz_offset = (int16_t)I2CReceive16(I2C_IMU, MPU_I2C_ADDR, MPU_GYRO_Z_OFFS_H);
}

void IMU_measumerements_for_calib(int16_t *mean_gx, int16_t *mean_gy, int16_t *mean_gz){
	int i = 0;
	int16_t gyro_x, gyro_y, gyro_z;
	int16_t buffer_x = 0, buffer_y = 0, buffer_z = 0;
	/*Take 1000 measurements to obtain mean of the each reading, but discard first 100 measurement to give the sensor time to adjust */
	while(i<1101){
		IMU_read_gyro(&gyro_x, &gyro_y, &gyro_z);
		if(i > 100 && i<1101){
			buffer_x = buffer_x + gyro_x;
			buffer_y = buffer_y + gyro_y;
			buffer_z = buffer_z + gyro_z;
		}
		if(i==1000){
			*mean_gx = buffer_x/1000;
			*mean_gy = buffer_y/1000;
			*mean_gz = buffer_z/1000;
		}
		i++;
		Delay_ms(40);	//wait 40ms so we don't get repeated measurements; 50ms is chosn because the highest delay between measurements for BW 5Hz is 33.48ms -> p.13 Register Map Document
	}
	UART_puts(UART_DEBUG_4, "Measurements taken\r\n");
}