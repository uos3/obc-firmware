
// i2c IMU functionality test
//
// Suzanna Lucarotti (c) 11/8/2017
//
// for use with the UoS3 Cubesat
//
// derived from blinky and Ti's i2c examples and MPU9205 datasheets
//
// this version alters the format to transmit for a python automated plotter

// packet format is 0xFE (frame start), then Acc data (6 bytes x,y,z) , Gyr data (6bytes x,y,z,), temp (2bytes), Magnetometer (6bytes x,y,z) 
// all data is big endian


#include "../firmware.h"

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include <stdlib.h>
#include <stdarg.h>

#define DEBUG_SERIAL UART_CAMERA // this is the serial port debug messages will come through (UART_CAMERA or UART_GNSS)
#define GPS_SERIAL UART_GNSS
#define CAM_SERIAL UART_CAMERA

#define DEBUG_SERIAL GPS_SERIAL

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

 
// Wrappers for printing to serial port - always to the one specified as output, placed here close to code for clarity

#define STRING_BUFFER_LENGTH 20 // rough hack for testing

static char string_buffer[STRING_BUFFER_LENGTH];
static char string_buffer2[STRING_BUFFER_LENGTH];

static void UART_putnum(unsigned int serialport,signed long x)
 {
  itoa(abs(x),string_buffer,16); // 16 for hex, 10 for decimal
  unsigned int len=strlen(string_buffer);
  unsigned int targetlen=6-len;
  strcpy(string_buffer2,"+00000");
  strcpy(string_buffer2+targetlen,string_buffer);
  if (x<0) string_buffer2[0]='-';
  UART_puts(serialport,string_buffer2);
}

static void UART_putstr(unsigned int serialport, char *s1,signed long x, char *s2)
 {
  if (s1!=NULL) UART_puts(serialport,s1);
  UART_putnum(serialport,x);
  if (s2!=NULL) UART_puts(serialport,s2);
 }  

#define DISP3(x,y,z) UART_putstr(DEBUG_SERIAL,x,y,z); // x and z are strings, y is a number shown as signed base10 16bit
#define DISP2(y,z) UART_putstr(DEBUG_SERIAL,NULL,y,z); 
#define DISP1(x) UART_puts(DEBUG_SERIAL,x);
#define DISP(x) UART_putc(DEBUG_SERIAL,x);
#define DISPW(x) UART_putc(DEBUG_SERIAL,x&0xff);UART_putc(DEBUG_SERIAL,x>>8);

 //////////////////////////////////////////////////////////////////
 // the actual main code
 //////////////////////////////////////////////////////////////////

int main(void)
{  

  Board_init(); // start the board
  InitI2C2(); // initialise correct interface on processor
  setupwatchdoginterrupt();
  
  UART_init(DEBUG_SERIAL, 115200);   //DISP1("\n\n\r   I2C Satellite I2C IMU test.\n")

 // start the console, print bootup message (below)

 #define MAG_PASS_THROUGH_I2C_ADDR 12 

 #define MAG_HXL 3
 #define MAG_HYL 5
 #define MAG_HZL 7 // offsets in magnetometer, note little endian, so need to be read opposite way round
 #define MAG_CNTL1 10 // Magnetometer control register
 #define MAG_STA1 2 // read only gives flags at 1 for DRDY and 2 for DOR (data ready and overload), used in read cycle

// enable magnetometer to appear separately I2C by commanding MPU chip

 char i2cstring[3]; // static buffer for i2c calls here
 i2cstring[0]=MPU_INT_BYPASS_ENABLE; // int pin /bypass enable configuration
 char i2cstatus=I2CReceive(SLAVE_ADDRESS,MPU_INT_BYPASS_ENABLE,I2C2_BASE);
 i2cstring[1]=i2cstatus | MPU_INT_BYPASS_ENABLE; // flag bypass on
 i2cstring[3]=0; // null terminated string
 I2CSendString(SLAVE_ADDRESS, i2cstring,I2C2_BASE); // turn on bypass to Magnetometer so visible on I2C

// setup magnetometer

 i2cstring[0]=MAG_CNTL1;
 i2cstring[2]=0;
 i2cstring[1]=0; // set mode to zero before changing mode
 I2CSendString(MAG_PASS_THROUGH_I2C_ADDR, i2cstring,I2C2_BASE); // set Magnetometer to safe mode before mode change 
 i2cstring[1]=1 | 0; // continuous (16hz) mode with 14bit range
 I2CSendString(MAG_PASS_THROUGH_I2C_ADDR, i2cstring,I2C2_BASE); // set Magnetometer to continuous shot mode with 16bit range, single shot needs updating and has delay 

 char mag_id=I2CReceive(MAG_PASS_THROUGH_I2C_ADDR,0,I2C2_BASE); // magnetometer ID (hopefully)

//DISP3("\r I2Cstatus old: ",i2cstatus,"\n")
//DISP3("\r I2Cstatus new (After enabling passthrough for Magnetometer): ",I2CReceive(SLAVE_ADDRESS,55),"\n")
//DISP3("\r Magnetometer ID (test, should equal 72 if found): ",mag_id,"\n")

signed short acc_x,acc_y,acc_z,gyr_x,gyr_y,gyr_z,mag_x,mag_y,mag_z,temp;

// some macros to simplify repeated I2c calls - deliberately kept near where used
#define MPUGET(x) I2CReceive16(SLAVE_ADDRESS,x,I2C2_BASE); // wrapper for I2C call to MPU
#define MAGGET(x) I2CReceive16r(MAG_PASS_THROUGH_I2C_ADDR, x,I2C2_BASE); // wrapper for I2C call to Magnetometer

DISP1("\n\n\n\rI2C IMU test - python compressed data version (use I2Ctest for human readable):\n\n")
wait(1000); // so can see this if monitoring in terminal

  while(1) // infinite loop
 {    
    acc_x=MPUGET(MPU_ACCEL_XOUT) acc_y=MPUGET( MPU_ACCEL_YOUT) acc_z=MPUGET(MPU_ACCEL_ZOUT)
    gyr_x=MPUGET(MPU_GYRO_XOUT) gyr_y=MPUGET(MPU_GYRO_YOUT) gyr_z=MPUGET(MPU_GYRO_ZOUT)
    temp=MPUGET(MPU_TEMP_OUT)
    
    // magnetometer should be in power down mode with data ready, if not wait for it
    while ((I2CReceive(MAG_PASS_THROUGH_I2C_ADDR,MAG_STA1,I2C2_BASE)&1)!=1) {}
    mag_x=MAGGET(MAG_HXL) mag_y=MAGGET(MAG_HYL)  mag_z=MAGGET(MAG_HZL)

    I2CSendString(MAG_PASS_THROUGH_I2C_ADDR, i2cstring,I2C2_BASE); // prepare magnetometer for next call (delay, so give lead in
    
   // now send the packets for python

    DISP('@') // frame start
    DISPW(acc_x) DISPW(acc_y) DISPW(acc_z)
    DISPW(gyr_x) DISPW(gyr_y) DISPW(gyr_z)
    DISPW(mag_x) DISPW(mag_y) DISPW(mag_z)
   // DISPW(20000) DISPW(-20000) DISPW (10000)
    DISPW(temp)
 
}
}


