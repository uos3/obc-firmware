// i2c test built on back of below test demo (originally master_slave_loopback.c) and blinky merged and forged to make something working


// this is blinky working code

/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include "i2ctest.h" // local header

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

/* A very simple example that blinks the on-board LED. */

/*

int main(void)
{
  Board_init();
  WDT_kick();

  UART_init(UART_CAM_HEADER, 9600);

  while(1)
  {
    LED_on(LED_B); 
UART_puts(UART_CAM_HEADER, "LED On\r\n");
    Delay_ms(500);
    WDT_kick();

    LED_off(LED_B);
UART_puts(UART_CAM_HEADER, "LED Off\r\n");
     
    Delay_ms(500);
    WDT_kick();
  }
}

*/

void UARTprintf(x)               // lets use our console instead of TI one
 {UART_puts(UART_CAM_HEADER,x);UART_puts(UART_CAM_HEADER,"\r");}

// hack it to work with gyro and satellite i2c system (WIP)

// introduce below step by step (currently not even linking properly)


//*****************************************************************************
//
//! \addtogroup i2c_examples_list
//! <h1>I2C Master Loopback (i2c_master_slave_loopback)</h1>
//!
//! This example shows how to configure the I2C2 module for loopback mode.
//! This includes setting up the master and slave module.  Loopback mode
//! internally connects the master and slave data and clock lines together.
//! The address of the slave module is set in order to read data from the
//! master.  Then the data is checked to make sure the received data matches
//! the data that was transmitted.  This example uses a polling method for
//! sending and receiving data.
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - I2C2 peripheral
//! - GPIO Port B peripheral (for I2C2 pins)
//! - I2C2SCL - PB2
//! - I2C2SDA - PB3
//!
//! The following UART signals are configured only for displaying console
//! messages for this example.  These are not required for operation of I2C.
//! - UART0 peripheral
//! - GPIO Port A peripheral (for UART0 pins)
//! - UART0RX - PA0
//! - UART0TX - PA1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//
//*****************************************************************************

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

//initialize I2C module 0
//Slightly modified version of TI's example code
void InitI2C2(void)
{
    //enable I2C module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
 
    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
     
    //enable GPIO peripheral that contains I2C 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
 
    // Configure the pin muxing for I2C2 functions on port B2 and B3.
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

int16_t I2CReceive16(uint32_t slave_addr,uint8_t reg)
 {
  int16_t x=I2CReceive(slave_addr,reg);
  x=x<<8;
  x+=I2CReceive(slave_addr,reg+1);
 return x;
 }

//(I2CReceive(SLAVE_ADDRESS, 59)<<8) + I2CReceive(SLAVE_ADDRESS, 60);

//*****************************************************************************
//
// Number of I2C data packets to send.
//
//*****************************************************************************
#define NUM_I2C_DATA 3

//*****************************************************************************
//
// Set the address for slave module. This is a 7-bit address sent in the
// following format:
//                      [A6:A5:A4:A3:A2:A1:A0:RS]
//
// A zero in the "RS" position of the first byte means that the master
// transmits (sends) data to the selected slave, and a one in this position
// means that the master receives data from the slave.
//
//*****************************************************************************

#define STRING_BUFFER_LENGTH 20

static char string_buffer[STRING_BUFFER_LENGTH];

int main(void)
{
    uint32_t pui32DataTx[NUM_I2C_DATA];
    uint32_t pui32DataRx[NUM_I2C_DATA];
    uint32_t ui32Index;


 

    //
    // Set the clocking to run directly from the external crystal/oscillator.
    // TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
    // crystal on your board.
    //

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

   InitI2C2();

/*
    //
    // The I2C2 peripheral must be enabled before use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

    //
    // For this example I2C2 is used with PortB[3:2].  The actual port and
    // pins used may be different on your part, consult the data sheet for
    // more information.  GPIO port B needs to be enabled so these pins can
    // be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Configure the pin muxing for I2C2 functions on port B2 and B3.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PB2_I2C2SCL);
    GPIOPinConfigure(GPIO_PB3_I2C2SDA);

    //
    // Select the I2C function for these pins.  This function will also
    // configure the GPIO pins pins for I2C operation, setting them to
    // open-drain operation with weak pull-ups.  Consult the data sheet
    // to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    //
    // Enable and initialize the I2C2 master module.  Use the system clock for
    // the I2C2 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.  For this example we will use a data rate of 100kbps.
    //

    I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);


    //
    // Set the slave address to SLAVE_ADDRESS.

  //  I2CSlaveInit(I2C2_BASE, SLAVE_ADDRESS);

    //
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.  Set the address to SLAVE_ADDRESS
    // (as set in the slave module).  The receive parameter is set to false
    // which indicates the I2C Master is initiating a writes to the slave.  If
    // true, that would indicate that the I2C Master is initiating reads from
    // the slave.
    //
    I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, false);

    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for I2C operation.
    //

*/
  Board_init();
  WDT_kick();

  UART_init(UART_CAM_HEADER, 9600);

    //
    // Display the example setup on the console.
    //
    UARTprintf("\n\rI2C Satellite I2C test ->\n");
    UARTprintf("   Module = I2C2\n");
    UARTprintf("   Mode = Single Send/Receive\n");
    UARTprintf("   Rate = 100kbps\n\n\n");

//#define SLAVE_ADDRESS 0xD0 // this is the MPU-9250A i2c address %1011000
unsigned long SLAVE_ADDRESS=0x68;

signed short acc_x,acc_y,acc_z,gyr_x,gyr_y,gyr_z,mag_x,mag_y,mag_z;

   while(1)
 {



    UART_puts(UART_CAM_HEADER,"\n\r IMU data =>> "); //itoa(SLAVE_ADDRESS,string_buffer,10);UART_puts(UART_CAM_HEADER,string_buffer);
    
  
    acc_x=I2CReceive16(SLAVE_ADDRESS,59); //(I2CReceive(SLAVE_ADDRESS, 59)<<8) + I2CReceive(SLAVE_ADDRESS, 60);
    acc_y=I2CReceive16(SLAVE_ADDRESS, 61); //<<8) + I2CReceive(SLAVE_ADDRESS, 62);
    acc_z=I2CReceive16(SLAVE_ADDRESS, 63);//<<8) + I2CReceive(SLAVE_ADDRESS, 64);
  
    UART_puts(UART_CAM_HEADER," Acc ( X reg = "); itoa(acc_x,string_buffer,10);UART_puts(UART_CAM_HEADER,string_buffer);
    UART_puts(UART_CAM_HEADER,": Y reg = "); itoa(acc_y,string_buffer,10);UART_puts(UART_CAM_HEADER,string_buffer);
    UART_puts(UART_CAM_HEADER,": Z reg = "); itoa(acc_z,string_buffer,10);UART_puts(UART_CAM_HEADER,string_buffer);
    UART_puts(UART_CAM_HEADER," ) ");

   //WDT_kick();
}
/*



    //
    // Initalize the data to send.
    //
    pui32DataTx[0] = 0;//'I'; // self test gyro regs x,y,z
    pui32DataTx[1] = 1;//'2';
    pui32DataTx[2] = 2;//'C';

    //
    // Initalize the receive buffer.
    //
    for(ui32Index = 0; ui32Index < NUM_I2C_DATA; ui32Index++)
    {
        pui32DataRx[ui32Index] = 0;
    }

    //
    // Indicate the direction of the data.
    //
    UARTprintf("Transferring from: Master -> Slave\n");

    //
    // Send 3 pieces of I2C data from the master to the slave.
    //
    for(ui32Index = 0; ui32Index < NUM_I2C_DATA; ui32Index++)
    {
        //
        // Display the data that the I2C2 master is transferring.
        //
        UART_puts(UART_CAM_HEADER,"  Sending: '");
        UART_putc(UART_CAM_HEADER,pui32DataTx[ui32Index]);
        UART_puts(UART_CAM_HEADER,"'\r\n");

        //
        // Place the data to be sent in the data register
        //
        I2CMasterDataPut(I2C2_BASE, pui32DataTx[ui32Index]);

        //
        // Initiate send of data from the master.  
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

        //
        // Wait until master module is done transferring.
        //
        while(I2CMasterBusy(I2C2_BASE))
        {
        }    

   }

    // So hopefully we now have

    //
    // Reset receive buffer.
    //
    for(ui32Index = 0; ui32Index < NUM_I2C_DATA; ui32Index++)
    {
        pui32DataRx[ui32Index] = 0;
    }

    //
    // Indicate the direction of the data.
    //
    UARTprintf("\n\nTransferring from: Slave -> Master\n");

    //
    // Modify the data direction to true, so that seeing the address will
    // indicate that the I2C Master is initiating a read from the slave.
    //
    I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, true);

    //
    // Do a dummy receive to make sure you don't get junk on the first receive.
    //
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
 
    for(ui32Index = 0; ui32Index < NUM_I2C_DATA; ui32Index++)
    {
    
        //
        // Tell the master to read data.
        //
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
   
        //
        // Read the data from the master.
        //
        pui32DataRx[ui32Index] = I2CMasterDataGet(I2C2_BASE);

        //
        // Display the data that the slave has received.
        //
        UART_puts(UART_CAM_HEADER,"Received: '");
        UART_putc(UART_CAM_HEADER,pui32DataRx[ui32Index]);
        UART_puts(UART_CAM_HEADER,"'\r\n");

    }

*/

    //
    // Tell the user that the test is done.
    //
    UARTprintf("\nDone.\n\n");

    //
    // Return no errors
    //
    return(0);
}


