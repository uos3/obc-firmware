
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



void InitI2C2(void)
{
    //enable I2C module 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
 
    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
     
    //enable GPIO peripheral that contains I2C 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
 
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

//sends an I2C command to the specified slave
void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...)
{
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false);
     
    //stores list of variable number of arguments
    va_list vargs;
     
    //specifies the va_list to "open" and the last fixed argument
    //so vargs knows where to start looking
    va_start(vargs, num_of_args);
     
    //put data to be sent into FIFO
    I2CMasterDataPut(I2C2_BASE, va_arg(vargs, uint32_t));
     
    //if there is only one argument, we only need to use the
    //single send I2C function
    if(num_of_args == 1)
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
         
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C2_BASE));
         
        //"close" variable argument list
        va_end(vargs);
    }
     
    //otherwise, we start transmission of multiple bytes on the
    //I2C bus
    else
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
         
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C2_BASE));
         
        //send num_of_args-2 pieces of data, using the
        //BURST_SEND_CONT command of the I2C module
        for(uint8_t i = 1; i < (num_of_args - 1); i++)
        {
            //put next piece of data into I2C FIFO
            I2CMasterDataPut(I2C2_BASE, va_arg(vargs, uint32_t));
            //send next data that was just placed into FIFO
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
     
            // Wait until MCU is done transferring.
            while(I2CMasterBusy(I2C2_BASE));
        }
     
        //put last piece of data into I2C FIFO
        I2CMasterDataPut(I2C2_BASE, va_arg(vargs, uint32_t));
        //send next data that was just placed into FIFO
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C2_BASE));
         
        //"close" variable args list
        va_end(vargs);
    }
}

//sends an array of data via I2C to the specified slave
void I2CSendString(uint32_t slave_addr, char array[])
{
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false);
     
    //put data to be sent into FIFO
    I2CMasterDataPut(I2C2_BASE, array[0]);
     
    //if there is only one argument, we only need to use the
    //single send I2C function
    if(array[1] == '\0')
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
         
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C2_BASE));
    }
     
    //otherwise, we start transmission of multiple bytes on the
    //I2C bus
    else
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
         
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C2_BASE));
         
        //initialize index into array
        uint8_t i = 1;
 
        //send num_of_args-2 pieces of data, using the
        //BURST_SEND_CONT command of the I2C module
        while(array[i + 1] != '\0')
        {
            //put next piece of data into I2C FIFO
            I2CMasterDataPut(I2C2_BASE, array[i++]);
 
            //send next data that was just placed into FIFO
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
     
            // Wait until MCU is done transferring.
            while(I2CMasterBusy(I2C2_BASE));
        }
     
        //put last piece of data into I2C FIFO
        I2CMasterDataPut(I2C2_BASE, array[i]);
 
        //send next data that was just placed into FIFO
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
 
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C2_BASE));
    }
}

//read specified register on slave device
uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg)
{
    //specify that we are writing (a register address) to the
    //slave device
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false);
 
    //specify register to be read
    I2CMasterDataPut(I2C2_BASE, reg);
 
    //send control byte and register address byte to slave device
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
     
    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C2_BASE));
     
    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, true);
     
    //send control byte and read from the register we
    //specified
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
     
    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C2_BASE));
     
    //return data pulled from the specified register
    return I2CMasterDataGet(I2C2_BASE);
}

int16_t I2CReceive16(uint32_t slave_addr,uint8_t reg) // lets get 2 byte value (TBD improve performance by data burst v single read)
 {
  int16_t x=I2CReceive(slave_addr,reg);
  x=x<<8;
  x+=I2CReceive(slave_addr,reg+1);
 return x;
 }


/// Fancy Console code to display without scrolling

#define STRING_BUFFER_LENGTH 20 // rough hack for testing

static char string_buffer[STRING_BUFFER_LENGTH];
static char string_buffer2[STRING_BUFFER_LENGTH];

void UART_putnum(unsigned int serialport,signed long x)
 {
  itoa(abs(x),string_buffer,10);
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

 //////////////////////////////////////////////////////////////////
 // the actual main code
 //////////////////////////////////////////////////////////////////

int main(void)
{

  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |  SYSCTL_XTAL_16MHZ); // set the system clock
  
  InitI2C2(); // initialise correct interface on processor

  Board_init(); // start the board
  WDT_kick();

  UART_init(UART_CAM_HEADER, 9600); // start the console, print bootup message (below)

  UART_puts(UART_CAM_HEADER,"\n\n\r   I2C Satellite I2C IMU test.\n");
  UART_puts(UART_CAM_HEADER,"\r   Module = I2C2\n");
  UART_puts(UART_CAM_HEADER,"\r   Mode = Single Send/Receive\n");
  UART_puts(UART_CAM_HEADER,"\r   Rate = 100kbps\n\n\n");

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
    
    UART_putstr(UART_CAM_HEADER,"\r Accel:(",acc_x,",");
    UART_putstr(UART_CAM_HEADER,NULL,acc_y,",");
    UART_putstr(UART_CAM_HEADER,NULL,acc_z,") ");
    UART_putstr(UART_CAM_HEADER,"Gyro:(",gyr_x,",");
    UART_putstr(UART_CAM_HEADER,NULL,gyr_y,",");
    UART_putstr(UART_CAM_HEADER,NULL,gyr_z,") ");
    UART_putstr(UART_CAM_HEADER,"Temp:(",temp,")");
  }
  WDT_kick();
 }
}


