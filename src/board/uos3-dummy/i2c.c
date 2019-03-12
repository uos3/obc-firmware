
// i2c base functions, built over TI library functions - produce drivers for higher level functionality
//
// Suzanna Lucarotti (c) 31/8/2017
//
// for use with the UoS3 Cubesat
//
// hacked together from the various datasheets

//#include "../firmware.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>


#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"


#include "../i2c.h" // base definitions for functions

/* I2C physical port description struct */
typedef struct I2C_port_t {
  uint32_t peripheral_gpio;   // TI Driver GPIO Peripheral Reference
  uint32_t peripheral_i2c;    // TI Driver I2C Peripheral Reference
  uint32_t base_gpio;         // TI Driver GPIO Base Reference
  uint32_t base_i2c;          // TI Driver I2C Base Reference
  uint32_t pin_scl_function;  // TI Driver I2C SCL Pin Reference
  uint32_t pin_sda_function;  // TI Driver I2C SDA Pin Reference
  uint8_t  pin_scl;           // TI Driver SCL Pin Reference
  uint8_t  pin_sda;           // TI Driver SDA Pin Reference
  bool     full_speed;        // Standard Speed = 100KHz, Full Speed = 400KHz
  bool     initialised;
} I2C_port_t;

static I2C_port_t I2C_ports[2] = 
  {
    { 
      SYSCTL_PERIPH_GPIOB,
      SYSCTL_PERIPH_I2C0,
      GPIO_PORTB_BASE,
      I2C0_BASE,
      GPIO_PB2_I2C0SCL,
      GPIO_PB3_I2C0SDA,
      GPIO_PIN_2,
      GPIO_PIN_3,
      false,
      false
    },
    { 
      SYSCTL_PERIPH_GPIOE,
      SYSCTL_PERIPH_I2C2,
      GPIO_PORTE_BASE,
      I2C2_BASE,
      GPIO_PE4_I2C2SCL,
      GPIO_PE5_I2C2SDA,
      GPIO_PIN_4,
      GPIO_PIN_5,
      false,
      false
    }
  };

#define NUMBER_OF_I2C_PORTS  ( sizeof(I2C_ports) / sizeof(I2C_port_t) )

#define check_i2c_num(x, y)  if(x >= NUMBER_OF_I2C_PORTS) { return y; }

/** Public Functions */

void I2C_init(uint8_t i2c_num)
{
  check_i2c_num(i2c_num,);
  I2C_port_t *i2c = &I2C_ports[i2c_num];

  /* Check Virtual SPI is initialised */
  if(!i2c->initialised)
  {
    /* Initialise SPI Peripheral if not already initialised */
    if(!SysCtlPeripheralReady(i2c->peripheral_i2c))
    {
      SysCtlPeripheralReset(i2c->peripheral_gpio);
      SysCtlPeripheralEnable(i2c->peripheral_i2c);
      while(!SysCtlPeripheralReady(i2c->peripheral_i2c)) { };
    }

    /* Initialise GPIO Peripheral if not already initialised */
    if(!SysCtlPeripheralReady(i2c->peripheral_gpio))
    {
      SysCtlPeripheralReset(i2c->peripheral_gpio);
      SysCtlPeripheralEnable(i2c->peripheral_gpio);
      while(!SysCtlPeripheralReady(i2c->peripheral_gpio)) { };
    }

    /* Configure Pins for I2C function */
    GPIOPinConfigure(i2c->pin_sda_function);
    GPIOPinConfigure(i2c->pin_scl_function);

    /* Assign pins to I2C peripheral */
    GPIOPinTypeI2C(i2c->base_gpio, i2c->pin_sda);
    GPIOPinTypeI2CSCL(i2c->base_gpio, i2c->pin_scl);

    /* Set peripheral clockrate */
    I2CMasterInitExpClk(i2c->base_i2c, SysCtlClockGet(), i2c->full_speed);

    I2CMasterEnable(i2c->base_i2c);

    /* Clear current pending data */
    I2CRxFIFOFlush(i2c->base_i2c);
    I2CTxFIFOFlush(i2c->base_i2c);

    i2c->initialised = true;
  }
}


/////////////////////////////////////////////////////////////


#define I2CRECVMAXBUFFERLEN 4

static uint8_t I2CReceiveBuffer[I2CRECVMAXBUFFERLEN+1]; // this is the receive array for receipt of data

/////////////////////////////////////////////////////////////


//sends an array of data via I2C to the specified slave, zero terminated as a string.
void I2CSendString(uint8_t i2c_num, uint8_t slave_addr, uint8_t array[])
{
  check_i2c_num(i2c_num,);
  I2C_port_t *i2c = &I2C_ports[i2c_num];

  I2C_init(i2c_num);

    I2CMasterSlaveAddrSet(i2c->base_i2c, slave_addr, false); // tell it which slave to talk to
    I2CMasterDataPut(i2c->base_i2c, array[0]);
     
     if(array[1] == 0) // single byte mode
    {
        I2CMasterControl(i2c->base_i2c, I2C_MASTER_CMD_SINGLE_SEND);
        while(I2CMasterBusy(i2c->base_i2c)); // wait
    }
     
    else // multi byte mode
    {
        I2CMasterControl(i2c->base_i2c, I2C_MASTER_CMD_BURST_SEND_START);        
        while(I2CMasterBusy(i2c->base_i2c)); //wait
        uint8_t i = 1;
        while(array[i + 1] != 0) // keep going until find the end
        {
          I2CMasterDataPut(i2c->base_i2c, array[i++]);
          I2CMasterControl(i2c->base_i2c, I2C_MASTER_CMD_BURST_SEND_CONT);
          while(I2CMasterBusy(i2c->base_i2c));
        }    
        I2CMasterDataPut(i2c->base_i2c, array[i]);
        I2CMasterControl(i2c->base_i2c, I2C_MASTER_CMD_BURST_SEND_FINISH);
        while(I2CMasterBusy(i2c->base_i2c));
     
       if (I2CMasterErr(i2c->base_i2c)) // error handling to stop I2C freezing if glitches during transmission
               I2CMasterControl(i2c->base_i2c,I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);       
   }
}

 // this is the guts of the function, called by intermediate functions to get around C's lack of function overloading

uint8_t *I2CReceiveGP(uint8_t i2c_num, uint8_t slave_addr, uint8_t reg,uint8_t strlen, bool reverse) // returns zero pointer if an error...
 {
  check_i2c_num(i2c_num,NULL);
  I2C_port_t *i2c = &I2C_ports[i2c_num];

  I2C_init(i2c_num);

  // else pointer to zero terminated string of bytes read, reversed if requested
  // this should fill buffer exactly with no overrun, max buffer length defined as shown
  if (strlen>I2CRECVMAXBUFFERLEN || strlen<=0) { return NULL;}
  // before can receive data, have to tell slave what register to start at
    I2CMasterSlaveAddrSet(i2c->base_i2c, slave_addr, false); // set address to send to
    I2CMasterDataPut(i2c->base_i2c, reg);     // tell it register to be read
    I2CMasterControl(i2c->base_i2c, I2C_MASTER_CMD_SINGLE_SEND); // send message to prepare for read
     
    while(I2CMasterBusy(i2c->base_i2c)); // wait for I2c ready
     
    I2CMasterSlaveAddrSet(i2c->base_i2c, slave_addr, true);  // set address to read from
    
    if (strlen==1) { 
                     I2CMasterControl(i2c->base_i2c, I2C_MASTER_CMD_SINGLE_RECEIVE);
                     while(I2CMasterBusy(i2c->base_i2c));
                     I2CReceiveBuffer[0]=(uint8_t) I2CMasterDataGet(i2c->base_i2c); // exit with ptr to zero terminated two byte string
                     I2CReceiveBuffer[1]=0;
                     return I2CReceiveBuffer;
                     }
     else // we want a burst from the slave, not just one byte, slave interprets burst reads as starting from given reg and ascending
        {
        I2CMasterControl(i2c->base_i2c, I2C_MASTER_CMD_BURST_RECEIVE_START);
         while(I2CMasterBusy(i2c->base_i2c));
        I2CReceiveBuffer[0]=(uint8_t) I2CMasterDataGet(i2c->base_i2c);
        uint8_t i=1;
        while (i<strlen-1) // do the middle bytes
         { 
          while(I2CMasterBusy(i2c->base_i2c)); // wait for ready 
          I2CMasterControl(i2c->base_i2c, I2C_MASTER_CMD_BURST_RECEIVE_CONT);         
          if (I2CMasterErr(i2c->base_i2c)) // error handling
            {I2CMasterControl(i2c->base_i2c,I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);return NULL;} 
          I2CReceiveBuffer[i]=(uint8_t) I2CMasterDataGet(i2c->base_i2c);        
           i++;
         }

         I2CMasterControl(i2c->base_i2c, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
         while(I2CMasterBusy(i2c->base_i2c));
         I2CReceiveBuffer[strlen-1]=(uint8_t) I2CMasterDataGet(i2c->base_i2c);
         I2CReceiveBuffer[strlen]=0;
  
    if (I2CMasterErr(i2c->base_i2c)) // error handling to stop I2C interface freezing if glitches during transmission
               {I2CMasterControl(i2c->base_i2c,I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);return NULL;}       
         }      

  if (reverse==false) // handle big endian or little endian data
   for (uint8_t i=0;i<(strlen/2);i++) // quick bytewise reverse
    {uint8_t b=I2CReceiveBuffer[i];I2CReceiveBuffer[i]=I2CReceiveBuffer[strlen-i-1];I2CReceiveBuffer[strlen-i-1]=b;}
  return I2CReceiveBuffer;
 }

uint8_t I2CReceive(uint8_t i2c_num, uint8_t slave_addr, uint8_t reg)
{
  check_i2c_num(i2c_num,0);
  I2C_init(i2c_num);

  uint8_t *ans=I2CReceiveGP(i2c_num, slave_addr,reg,1,false);

  if (ans==NULL) return 0; // error handling
  return ans[0];
 }

uint16_t I2CReceive16(uint8_t i2c_num, uint8_t slave_addr,uint8_t reg) // lets get 2 byte value 
 {
  check_i2c_num(i2c_num,0);
  I2C_init(i2c_num);

  uint8_t *ans=I2CReceiveGP(i2c_num, slave_addr,reg,2,false);
  if (ans==NULL) return 0; // error handling
  //return ans[0]|(ans[1]<<8); // same as below, but below better
  return *(uint16_t *)ans;
 }

uint16_t I2CReceive16r(uint8_t i2c_num, uint8_t slave_addr,uint8_t reg) // lets get 2 byte value reversed (for other endian words)
 {
  check_i2c_num(i2c_num,0);
  I2C_init(i2c_num);

  uint8_t *ans=I2CReceiveGP(i2c_num, slave_addr,reg,2,true);
  if (ans==NULL) return 0; // error handling
  return *(uint16_t *)ans;
 }



