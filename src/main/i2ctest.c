
// i2c IMU functionality test
//
// Suzanna Lucarotti (c) 11/8/2017
//
// for use with the UoS3 Cubesat
//
// derived from blinky and Ti's i2c examples and MPU9205 datasheets


#include "../firmware.h"

//#include "i2ctest.h" // local header

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include <stdlib.h>
#include <stdarg.h>

#define DEBUG_SERIAL UART_CAM_HEADER // this is the serial port debug messages will come through (UART_CAM_HEADER or UART_PC104_HEADER)
#define GPS_SERIAL UART_PC104_HEADER
#define CAM_SERIAL UART_CAM_HEADER

#define DEBUG_SERIAL CAM_SERIAL


void InitI2C2(void) // initialise board for I2C capability
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);//enable I2C module 2
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);//reset module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);//enable GPIO peripheral that contains I2C 2
 
    // Configure the pin muxing for I2C2 functions on port E4 and E5.
    GPIOPinConfigure(GPIO_PE4_I2C2SCL);
    GPIOPinConfigure(GPIO_PE5_I2C2SDA);
     
    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);
 
    // Enable and initialize the I2C2 master module.  Use the system clock for
    // the I2C2 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false); 
     
    //clear I2C FIFOs
    HWREG(I2C2_BASE + I2C_O_FIFOCTL) = 80008000;
}


//sends an array of data via I2C to the specified slave, zero terminated as a string.
void I2CSendString(uint32_t slave_addr, char array[])
{
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false); // tell it which slave to talk to
    I2CMasterDataPut(I2C2_BASE, array[0]);
     
     if(array[1] == 0) // single byte mode
    {
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        while(I2CMasterBusy(I2C2_BASE)); // wait
    }
     
    else // multi byte mode
    {
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);        
        while(I2CMasterBusy(I2C2_BASE)); //wait
         uint8_t i = 1;
        while(array[i + 1] != 0) // keep going until find the end
        {
          I2CMasterDataPut(I2C2_BASE, array[i++]);
          I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
          while(I2CMasterBusy(I2C2_BASE));
        }    
        I2CMasterDataPut(I2C2_BASE, array[i]);
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        while(I2CMasterBusy(I2C2_BASE));
     
       if (I2CMasterErr(I2C2_BASE)) // error handling to stop I2C freezing if glitches during transmission
               I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);       
   }
}

//read specified register on slave device


#define I2CRECVMAXBUFFERLEN 4

static char I2CReceiveBuffer[I2CRECVMAXBUFFERLEN+1];

char *I2CReceiveGP(uint32_t slave_addr, uint8_t reg,uint8_t strlen, bool reverse) // returns zero pointer if an error...
 {                                      // else pointer to zero terminated string of bytes read, reversed if requested
    // this should fill buffer exactly with no overrun, responsibility for sufficient buffer size with caller
 if (strlen>I2CRECVMAXBUFFERLEN || strlen<=0) { return NULL;}
 // before can receive data, have to tell slave what register to start at
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false); // set address to send to
    I2CMasterDataPut(I2C2_BASE, reg);     // tell it register to be read
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND); // send message to prepare for read
     
    while(I2CMasterBusy(I2C2_BASE)); // wait for I2c ready
     
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, true);  // set address to read from
    
    if (strlen==1) { 
                     I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
                     while(I2CMasterBusy(I2C2_BASE));
                     I2CReceiveBuffer[0]=I2CMasterDataGet(I2C2_BASE); // exit with ptr to zero terminated two byte string
                     I2CReceiveBuffer[1]=0;
                     return I2CReceiveBuffer;
                     }
     else // we want a burst from the slave, not just one byte, slave interprets burst reads as starting from given reg and ascending
        {
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
         while(I2CMasterBusy(I2C2_BASE));
        I2CReceiveBuffer[0]=I2CMasterDataGet(I2C2_BASE);
        uint8_t i=1;
        while (i<strlen-1) // do the middle bytes
         { 
        while(I2CMasterBusy(I2C2_BASE)); // wait for ready
           {
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);         
            if (I2CMasterErr(I2C2_BASE)) // error handling
                                      {I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);return NULL;} 
            I2CReceiveBuffer[i]=I2CMasterDataGet(I2C2_BASE);
           }
           i++;
         }

         I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
         while(I2CMasterBusy(I2C2_BASE));
         I2CReceiveBuffer[strlen-1]=I2CMasterDataGet(I2C2_BASE);
         I2CReceiveBuffer[strlen]=0;
  
    if (I2CMasterErr(I2C2_BASE)) // error handling to stop I2C freezing if glitches during transmission
               {I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);return NULL;}       
         }      
  //I2CReceiveBuffer[0]=7;
  //I2CReceiveBuffer[1]=3;
  //I2CReceiveBuffer[2]=0;

  if (reverse==false) // handle big endian or little endian data
   for (uint8_t i=0;i<(strlen/2);i++) // quick bytewise reverse
    {char b=I2CReceiveBuffer[i];I2CReceiveBuffer[i]=I2CReceiveBuffer[strlen-i-1];I2CReceiveBuffer[strlen-i-1]=b;}
  return I2CReceiveBuffer;
 }

uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg)
{
  char *ans=I2CReceiveGP(slave_addr,reg,1,false);
  if (ans==NULL) return 0; // error handling
  return ans[0];
 }

uint16_t I2CReceive16(uint32_t slave_addr,uint8_t reg) // lets get 2 byte value (TBD improve performance by data burst v single read)
 {

 char *ans=I2CReceiveGP(slave_addr,reg,2,false);
  if (ans==NULL) return 0; // error handling
 //return ans[0]|(ans[1]<<8); // same as below, but below better
 return *(uint16_t *)ans;
 }

uint16_t I2CReceive16r(uint32_t slave_addr,uint8_t reg) // lets get 2 byte value (TBD improve performance by data burst v single read)
 {
 char *ans=I2CReceiveGP(slave_addr,reg,2,true);
  if (ans==NULL) return 0; // error handling
 return *(uint16_t *)ans;
 
/* old code doing it with 2 calls to one byte
  char *ans=I2CReceiveGP(slave_addr,reg,1,false);
   if (ans==NULL) return 0; // error handling

  int16_t x=ans[0];
  ans=I2CReceiveGP(slave_addr,reg+1,1,false);
  if (ans==NULL) return 0; // error handling

  x+=ans[0]*256;
 return x;
*/
 }

/// Fancy Console code to display without scrolling

#define STRING_BUFFER_LENGTH 20 // rough hack for testing

static char string_buffer[STRING_BUFFER_LENGTH];
static char string_buffer2[STRING_BUFFER_LENGTH];

void UART_putnum(unsigned int serialport,signed long x)
 {
  itoa(abs(x),string_buffer,10); // 16 for hex, 10 for decimal
  unsigned int len=strlen(string_buffer);
  unsigned int targetlen=6-len;
  strcpy(string_buffer2,"+00000");
  strcpy(string_buffer2+targetlen,string_buffer);
  if (x<0) string_buffer2[0]='-';
  UART_puts(serialport,string_buffer2);
}

void UART_putstr(unsigned int serialport, char *s1,signed long x, char *s2)
 {
  if (s1!=NULL) UART_puts(serialport,s1);
  UART_putnum(serialport,x);
  if (s2!=NULL) UART_puts(serialport,s2);
 }  

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

 //////////////////////////////////////////////////////////////////
 // the actual main code
 //////////////////////////////////////////////////////////////////

int main(void)
{

  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |  SYSCTL_XTAL_16MHZ); // set the system clock
  
  InitI2C2(); // initialise correct interface on processor

  Board_init(); // start the board
  WDT_kick();

  UART_init(DEBUG_SERIAL, 9600); // start the console, print bootup message (below)

  UART_puts(DEBUG_SERIAL,"\n\n\r   I2C Satellite I2C IMU test.\n");

 #define MAG_PASS_THROUGH_I2C_ADDR 12 

 #define MAG_HXL 3
 #define MAG_HYL 5
 #define MAG_HZL 7 // offsets in magnetometer, note little endian, so need to be read opposite way round
 #define MAG_CNTL1 10 // Magnetometer control register
 #define MAG_STA1 2 // read only gives flags at 1 for DRDY and 2 for DOR (data ready and overload), used in read cycle

 char i2cstring[3];
 i2cstring[0]=MPU_INT_BYPASS_ENABLE; // int pin /bypass enable configuration
 //i2cstring[1]=2; // turn on bypass enable

 char i2cstatus=I2CReceive(SLAVE_ADDRESS,55);
 i2cstring[1]=i2cstatus | 2; // flag bypass on
 i2cstring[3]=0; // null terminated string
 
 I2CSendString(SLAVE_ADDRESS, i2cstring); // turn on bypass to Magnetometer so visible on I2C

 i2cstring[0]=MAG_CNTL1;
 i2cstring[2]=0;
 i2cstring[1]=0; // set mode to zero before changing mode
 I2CSendString(MAG_PASS_THROUGH_I2C_ADDR, i2cstring); // set Magnetometer to safe mode before mode change 
 i2cstring[1]=1 | 0; // continuous (16hz) mode with 14bit range
 I2CSendString(MAG_PASS_THROUGH_I2C_ADDR, i2cstring); // set Magnetometer to continuous shot mode with 16bit range, single shot needs updating and has delay 

 char mag_id=I2CReceive(MAG_PASS_THROUGH_I2C_ADDR,0); // magnetometer ID (hopefully)

UART_putstr(DEBUG_SERIAL,"\r I2Cstatus old: ",i2cstatus,"\n");
UART_putstr(DEBUG_SERIAL,"\r I2Cstatus new (After enabling passthrough for Magnetometer): ",I2CReceive(SLAVE_ADDRESS,55),"\n");
UART_putstr(DEBUG_SERIAL,"\r Magnetometer ID (test, should equal 72 if found): ",mag_id,"\n");

signed short acc_x,acc_y,acc_z,gyr_x,gyr_y,gyr_z,mag_x,mag_y,mag_z,temp;

unsigned int wdt_start=20; // loops before kick, not too long or too short or hardware will reset

   while(1) // infinite loop
 {

 for (unsigned int wdt_kicker=wdt_start;wdt_kicker>0;wdt_kicker--) // repeat this to kick wdt at correct time.
  {
     
    acc_x=I2CReceive16(SLAVE_ADDRESS,MPU_ACCEL_XOUT); 
    acc_y=I2CReceive16(SLAVE_ADDRESS, MPU_ACCEL_YOUT); 
    acc_z=I2CReceive16(SLAVE_ADDRESS, MPU_ACCEL_ZOUT);
    gyr_x=I2CReceive16(SLAVE_ADDRESS, MPU_GYRO_XOUT);
    gyr_y=I2CReceive16(SLAVE_ADDRESS, MPU_GYRO_YOUT);
    gyr_z=I2CReceive16(SLAVE_ADDRESS, MPU_GYRO_ZOUT);
    temp=I2CReceive16(SLAVE_ADDRESS, MPU_TEMP_OUT);
    
    // magnetometer should be in power down mode with data ready, if not wait for it
    while ((I2CReceive(MAG_PASS_THROUGH_I2C_ADDR,MAG_STA1)&1)!=1) {}
    mag_x=I2CReceive16r(MAG_PASS_THROUGH_I2C_ADDR, MAG_HXL); 
    mag_y=I2CReceive16r(MAG_PASS_THROUGH_I2C_ADDR, MAG_HYL); 
    mag_z=I2CReceive16r(MAG_PASS_THROUGH_I2C_ADDR, MAG_HZL);

    I2CSendString(MAG_PASS_THROUGH_I2C_ADDR, i2cstring); // prepare for next call (delay, so give lead in
    
    UART_putstr(DEBUG_SERIAL,"\r A:(",acc_x,",");
    UART_putstr(DEBUG_SERIAL,NULL,acc_y,",");
    UART_putstr(DEBUG_SERIAL,NULL,acc_z,") ");
    UART_putstr(DEBUG_SERIAL,"G:(",gyr_x,",");
    UART_putstr(DEBUG_SERIAL,NULL,gyr_y,",");
    UART_putstr(DEBUG_SERIAL,NULL,gyr_z,") ");
    UART_putstr(DEBUG_SERIAL,"Temp:(",temp,")");
    UART_putstr(DEBUG_SERIAL," M:(",mag_x,",");
    UART_putstr(DEBUG_SERIAL,NULL,mag_y,",");
    UART_putstr(DEBUG_SERIAL,NULL,mag_z,") ");

  }
  WDT_kick();
 }
}


