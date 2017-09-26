
// i2c base functions, built over TI library functions - produce drivers for higher level functionality
//
// Suzanna Lucarotti (c) 31/8/2017
//
// for use with the UoS3 Cubesat
//
// hacked together from the various datasheets


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

#include "i2cbase.h" // base definitions for functions


/////////////////////////////////////////////////////////////


#define I2CRECVMAXBUFFERLEN 4

static char I2CReceiveBuffer[I2CRECVMAXBUFFERLEN+1]; // this is the receive array for receipt of data

/////////////////////////////////////////////////////////////


void InitI2C(int i2cbase, int sysctl_i2c,int sysctl_gpio,int gpio_port,int pin_scl,int pin_sda,int mux_scl, int mux_sda) // initialise board for I2C capability
{
 //   SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |  SYSCTL_XTAL_16MHZ); // set the system clock
    
    SysCtlPeripheralEnable(sysctl_i2c);//enable I2C module 2
    SysCtlPeripheralReset(sysctl_i2c);//reset module
    SysCtlPeripheralEnable(sysctl_gpio);//enable GPIO peripheral that contains I2C 2
 
    // Configure the pin muxing for I2C2 functions on port E4 and E5.
    GPIOPinConfigure(pin_scl);
    GPIOPinConfigure(pin_sda);
     
    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(gpio_port, mux_scl);
    GPIOPinTypeI2C(gpio_port, mux_sda);
 
    // Enable and initialize the I2C2 master module.  Use the system clock for
    // the I2C2 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(i2cbase, SysCtlClockGet(), false); 
     
    //clear I2C FIFOs
    HWREG(i2cbase + I2C_O_FIFOCTL) = 80008000;
}


void InitI2C2() // initialise board for I2C capability
{
  InitI2C(I2C2_BASE,SYSCTL_PERIPH_I2C2,SYSCTL_PERIPH_GPIOE,GPIO_PORTE_BASE,GPIO_PE4_I2C2SCL,GPIO_PE5_I2C2SDA,GPIO_PIN_4,GPIO_PIN_5);
}

void InitI2C0() // initialise board for I2C capability
{
  InitI2C(I2C0_BASE,SYSCTL_PERIPH_I2C0,SYSCTL_PERIPH_GPIOB,GPIO_PORTB_BASE,GPIO_PB2_I2C0SCL,GPIO_PB3_I2C0SDA,GPIO_PIN_2,GPIO_PIN_3);
}


//sends an array of data via I2C to the specified slave, zero terminated as a string.
void I2CSendString(uint32_t slave_addr, char array[],int i2cbase)
{
    I2CMasterSlaveAddrSet(i2cbase, slave_addr, false); // tell it which slave to talk to
    I2CMasterDataPut(i2cbase, array[0]);
     
     if(array[1] == 0) // single byte mode
    {
        I2CMasterControl(i2cbase, I2C_MASTER_CMD_SINGLE_SEND);
        while(I2CMasterBusy(i2cbase)); // wait
    }
     
    else // multi byte mode
    {
        I2CMasterControl(i2cbase, I2C_MASTER_CMD_BURST_SEND_START);        
        while(I2CMasterBusy(i2cbase)); //wait
         uint8_t i = 1;
        while(array[i + 1] != 0) // keep going until find the end
        {
          I2CMasterDataPut(i2cbase, array[i++]);
          I2CMasterControl(i2cbase, I2C_MASTER_CMD_BURST_SEND_CONT);
          while(I2CMasterBusy(i2cbase));
        }    
        I2CMasterDataPut(i2cbase, array[i]);
        I2CMasterControl(i2cbase, I2C_MASTER_CMD_BURST_SEND_FINISH);
        while(I2CMasterBusy(i2cbase));
     
       if (I2CMasterErr(i2cbase)) // error handling to stop I2C freezing if glitches during transmission
               I2CMasterControl(i2cbase,I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);       
   }
}

 // this is the guts of the function, called by intermediate functions to get around C's lack of function overloading


char *I2CReceiveGP(uint32_t slave_addr, uint8_t reg,uint8_t strlen, int i2cbase, bool reverse) // returns zero pointer if an error...
 {                                      // else pointer to zero terminated string of bytes read, reversed if requested
    // this should fill buffer exactly with no overrun, max buffer length defined as shown
 if (strlen>I2CRECVMAXBUFFERLEN || strlen<=0) { return NULL;}
 // before can receive data, have to tell slave what register to start at
    I2CMasterSlaveAddrSet(i2cbase, slave_addr, false); // set address to send to
    I2CMasterDataPut(i2cbase, reg);     // tell it register to be read
    I2CMasterControl(i2cbase, I2C_MASTER_CMD_SINGLE_SEND); // send message to prepare for read
     
    while(I2CMasterBusy(i2cbase)); // wait for I2c ready
     
    I2CMasterSlaveAddrSet(i2cbase, slave_addr, true);  // set address to read from
    
    if (strlen==1) { 
                     I2CMasterControl(i2cbase, I2C_MASTER_CMD_SINGLE_RECEIVE);
                     while(I2CMasterBusy(i2cbase));
                     I2CReceiveBuffer[0]=I2CMasterDataGet(i2cbase); // exit with ptr to zero terminated two byte string
                     I2CReceiveBuffer[1]=0;
                     return I2CReceiveBuffer;
                     }
     else // we want a burst from the slave, not just one byte, slave interprets burst reads as starting from given reg and ascending
        {
        I2CMasterControl(i2cbase, I2C_MASTER_CMD_BURST_RECEIVE_START);
         while(I2CMasterBusy(i2cbase));
        I2CReceiveBuffer[0]=I2CMasterDataGet(i2cbase);
        uint8_t i=1;
        while (i<strlen-1) // do the middle bytes
         { 
        while(I2CMasterBusy(i2cbase)); // wait for ready
           {
            I2CMasterControl(i2cbase, I2C_MASTER_CMD_BURST_RECEIVE_CONT);         
            if (I2CMasterErr(i2cbase)) // error handling
                                      {I2CMasterControl(i2cbase,I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);return NULL;} 
            I2CReceiveBuffer[i]=I2CMasterDataGet(i2cbase);
           }
           i++;
         }

         I2CMasterControl(i2cbase, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
         while(I2CMasterBusy(i2cbase));
         I2CReceiveBuffer[strlen-1]=I2CMasterDataGet(i2cbase);
         I2CReceiveBuffer[strlen]=0;
  
    if (I2CMasterErr(i2cbase)) // error handling to stop I2C interface freezing if glitches during transmission
               {I2CMasterControl(i2cbase,I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);return NULL;}       
         }      

  if (reverse==false) // handle big endian or little endian data
   for (uint8_t i=0;i<(strlen/2);i++) // quick bytewise reverse
    {char b=I2CReceiveBuffer[i];I2CReceiveBuffer[i]=I2CReceiveBuffer[strlen-i-1];I2CReceiveBuffer[strlen-i-1]=b;}
  return I2CReceiveBuffer;
 }

char I2CReceive(uint32_t slave_addr, uint8_t reg,int i2cbase)
{
  char *ans=I2CReceiveGP(slave_addr,reg,1,i2cbase,false);
  if (ans==NULL) return 0; // error handling
  return ans[0];
 }

uint16_t I2CReceive16(uint32_t slave_addr,uint8_t reg,int i2cbase) // lets get 2 byte value 
 {

 char *ans=I2CReceiveGP(slave_addr,reg,2,i2cbase,false);
  if (ans==NULL) return 0; // error handling
 //return ans[0]|(ans[1]<<8); // same as below, but below better
 return *(uint16_t *)ans;
 }

uint16_t I2CReceive16r(uint32_t slave_addr,uint8_t reg,int i2cbase) // lets get 2 byte value reversed (for other endian words)
 {
 char *ans=I2CReceiveGP(slave_addr,reg,2,i2cbase,true);
  if (ans==NULL) return 0; // error handling
 return *(uint16_t *)ans;
 }



