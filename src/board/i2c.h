
// i2c base functions, built over TI library functions - produce drivers for higher level functionality
//
// Suzanna Lucarotti (c) 31/8/2017
//
// for use with the UoS3 Cubesat
//
// hacked together from the various datasheets


#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>
#include <stdbool.h>

void InitI2C0(void); // initialise board for I2C capability - IMU
void InitI2C2(void); // initialise board for I2C capability - Radio temp sensors

uint8_t I2CReceive(uint8_t slave_addr, uint8_t reg,uint32_t i2cbase); // i2cbase is either I2C2_BASE or I2C0_BASE by wiring. Get a byte from reg
uint16_t I2CReceive16(uint8_t slave_addr,uint8_t reg,uint32_t i2cbase); // lets get 2 byte value 
uint16_t I2CReceive16r(uint8_t slave_addr,uint8_t reg,uint32_t i2cbase); // lets get 2 byte value reversed (for other endian words)

//sends an array of data via I2C to the specified slave, zero terminated as a string.
void I2CSendString(uint8_t slave_addr, uint8_t array[],uint32_t i2cbase);

 // this is the guts of the read function, called by intermediate functions to get around C's lack of function overloading
uint8_t *I2CReceiveGP(uint8_t slave_addr, uint8_t reg,uint8_t strlen,uint32_t i2cbase, bool reverse); // returns zero pointer if an error...

#endif // __I2C_H__
