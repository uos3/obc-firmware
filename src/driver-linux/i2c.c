#include <stdint.h>
#include <stdbool.h>
#include "../driver/i2c.h"

void I2C_init(uint8_t i2c_num){

}

//  Get a byte from reg
uint8_t I2CReceive(uint8_t i2c_num, uint8_t slave_addr, uint8_t reg){

}

// lets get 2 byte value
uint16_t I2CReceive16(uint8_t i2c_num, uint8_t slave_addr,uint8_t reg){

}

// lets get 2 byte value reversed (for other endian words)
uint16_t I2CReceive16r(uint8_t i2c_num, uint8_t slave_addr,uint8_t reg){

}

//sends an array of data via I2C to the specified slave, zero terminated as a string.
void I2CSendString(uint8_t i2c_num, uint8_t slave_addr, uint8_t array[]){

}

// this is the guts of the read function, called by intermediate functions to get around C's lack of function overloading
// returns zero pointer if an error...
uint8_t *I2CReceiveGP(uint8_t i2c_num, uint8_t slave_addr, uint8_t reg,uint8_t strlen, bool reverse){

}
