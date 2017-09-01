
// i2c base functions, built over TI library functions - produce drivers for higher level functionality
//
// Suzanna Lucarotti (c) 31/8/2017
//
// for use with the UoS3 Cubesat
//
// hacked together from the various datasheets

void InitI2C2(void); // initialise board for I2C capability
char I2CReceive(uint32_t slave_addr, uint8_t reg);
uint16_t I2CReceive16(uint32_t slave_addr,uint8_t reg); // lets get 2 byte value 
uint16_t I2CReceive16r(uint32_t slave_addr,uint8_t reg); // lets get 2 byte value reversed (for other endian words)

//sends an array of data via I2C to the specified slave, zero terminated as a string.
void I2CSendString(uint32_t slave_addr, char array[]);

 // this is the guts of the read function, called by intermediate functions to get around C's lack of function overloading
char *I2CReceiveGP(uint32_t slave_addr, uint8_t reg,uint8_t strlen, bool reverse); // returns zero pointer if an error...


