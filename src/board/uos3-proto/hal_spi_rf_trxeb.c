/******************************************************************************
*  Filename: hal_spi_rf_trxeb.c
*
*  Description: Implementation file for common spi access with the CCxxxx 
*               tranceiver radios using trxeb. Supports CC1101/CC112X radios
*
*  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/


/******************************************************************************
 * INCLUDES
 */
//#include <msp430.h>
//#include "hal_types.h"
//#include "hal_defs.h"
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"

#include "../hal_spi_rf_trxeb.h"

#include "../../firmware.h"

#include "board.h"



/******************************************************************************
 * LOCAL FUNCTIONS
 */
static void trxReadWriteBurstSingle(uint8_t addr,uint8_t *pData,uint16_t len) ;

static void ssi_flush(void)
{
	uint32_t d[8];
	SSIDataGetNonBlocking(SSI1_BASE , d );
}

static void cs_high(uint8_t radio_id){
	if (radio_id == RADIO_TX)
		GPIOPinWrite(RADIO_TX_CS_PORT, RADIO_TX_CS_PIN, RADIO_TX_CS_PIN);
	else //if (radio_id == RADIO_RX)
		GPIOPinWrite(RADIO_RX_CS_PORT, RADIO_RX_CS_PIN, RADIO_RX_CS_PIN);
}
static void cs_low(uint8_t radio_id){
	if (radio_id == RADIO_TX)
		GPIOPinWrite(RADIO_TX_CS_PORT, RADIO_TX_CS_PIN, 0);
	else //if (radio_id == RADIO_RX)
		GPIOPinWrite(RADIO_RX_CS_PORT, RADIO_RX_CS_PIN, 0);
}


/******************************************************************************
 * FUNCTIONS
 */

/******************************************************************************
 * @fn          trxRfSpiInterfaceInit
 *
 * @brief       Function to initialize TRX SPI. CC1101/CC112x is currently
 *              supported. The supported prescalerValue must be set so that
 *              SMCLK/prescalerValue does not violate radio SPI constraints.
 *
 * input parameters
 *
 * @param       prescalerValue - SMCLK/prescalerValue gives SCLK frequency
 *
 * output parameters
 *
 * @return      void
 */
void trxRfSpiInterfaceInit(uint8_t prescalerValue)
{

  
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	// sort out PF0...
	HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY; //Unlock 
	HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= 0x01; // Enable PF0 AFS 
	HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) =0; // Relock
	
	GPIOPinConfigure(GPIO_PF2_SSI1CLK);
    //GPIOPinConfigure(GPIO_PF3_SSI1FSS);
    GPIOPinConfigure(GPIO_PF0_SSI1RX);
    GPIOPinConfigure(GPIO_PF1_SSI1TX);
	GPIOPinTypeSSI(GPIO_PORTF_BASE, RADIO_MISO_PIN | RADIO_MOSI_PIN | RADIO_CLK_PIN);
	SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);
	
	
	SSIEnable(SSI1_BASE);
	
	uint32_t d;
	while(SSIDataGetNonBlocking(SSI1_BASE, &d));
	
	//configure CS pin (TX)
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(RADIO_TX_CS_PORT, RADIO_TX_CS_PIN);
	GPIOPinWrite(RADIO_TX_CS_PORT, RADIO_TX_CS_PIN, RADIO_TX_CS_PIN);
	
	//configure CS pin (RX)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOOutput(RADIO_RX_CS_PORT, RADIO_RX_CS_PIN);
	GPIOPinWrite(RADIO_RX_CS_PORT, RADIO_RX_CS_PIN, RADIO_RX_CS_PIN);

  
  
  return;
}


/*******************************************************************************
 * @fn          trx8BitRegAccess
 *
 * @brief       This function performs a read or write from/to a 8bit register
 *              address space. The function handles burst and single read/write
 *              as specfied in addrByte. Function assumes that chip is ready.
 *
 * input parameters
 *
 * @param       radio_id - which radio IC to communicate to
 * @param       accessType - Specifies if this is a read or write and if it's
 *                           a single or burst access. Bitmask made up of
 *                           RADIO_BURST_ACCESS/RADIO_SINGLE_ACCESS/
 *                           RADIO_WRITE_ACCESS/RADIO_READ_ACCESS.
 * @param       addrByte - address byte of register.
 * @param       pData    - data array
 * @param       len      - Length of array to be read(TX)/written(RX)
 *
 * output parameters
 *
 * @return      chip status
 */
rfStatus_t trx8BitRegAccess(uint8_t radio_id, uint8_t accessType, uint8_t addrByte, uint8_t *pData, uint16_t len)
{
  uint32_t readValue;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  cs_low(radio_id);
  //while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);   <-- do we need this..?
  while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) & GPIO_PIN_0){};
  // do we need to flush the rx buffer first?
  ssi_flush();
 
  while(SSIBusy(SSI1_BASE));
  SSIDataPut(SSI1_BASE, (uint32_t)(accessType|addrByte));
  while(SSIBusy(SSI1_BASE));
  SSIDataGet(SSI1_BASE, &readValue);
  trxReadWriteBurstSingle(accessType|addrByte,pData,len);   
  cs_high(radio_id);
  /* return the status byte value */
  return((uint8_t)readValue);
}

/******************************************************************************
 * @fn          trx16BitRegAccess
 *
 * @brief       This function performs a read or write in the extended adress
 *              space of CC112X.
 *
 * input parameters
 *
 * @param       radio_id - which radio IC to communicate to
 * @param       accessType - Specifies if this is a read or write and if it's
 *                           a single or burst access. Bitmask made up of
 *                           RADIO_BURST_ACCESS/RADIO_SINGLE_ACCESS/
 *                           RADIO_WRITE_ACCESS/RADIO_READ_ACCESS.
 * @param       extAddr - Extended register space address = 0x2F.
 * @param       regAddr - Register address in the extended address space.
 * @param       *pData  - Pointer to data array for communication
 * @param       len     - Length of bytes to be read/written from/to radio
 *
 * output parameters
 *
 * @return      rfStatus_t
 */
rfStatus_t trx16BitRegAccess(uint8_t radio_id, uint8_t accessType, uint8_t extAddr, uint8_t regAddr, uint8_t *pData, uint8_t len)
{
  uint32_t readValue;
  uint32_t d;
  
  cs_low(radio_id);
  
  while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) & GPIO_PIN_0){};
  // do we need to flush the rx buffer first?
  ssi_flush();
  
  while(SSIBusy(SSI1_BASE));
  SSIDataPut(SSI1_BASE, (uint32_t)(accessType|extAddr));
  while(SSIBusy(SSI1_BASE));
  SSIDataGet(SSI1_BASE, &readValue);
  while(SSIBusy(SSI1_BASE));
  SSIDataPut(SSI1_BASE, (uint32_t)(regAddr));
  while(SSIBusy(SSI1_BASE));
  // do we need a dummy read?
  SSIDataGet(SSI1_BASE, (uint32_t *)&d);
  
  trxReadWriteBurstSingle(accessType|extAddr,pData,len);
  
  
  cs_high(radio_id);
  
  
  return((uint8_t)readValue);
}






/*******************************************************************************
 * @fn          trxSpiCmdStrobe
 *
 * @brief       Send command strobe to the radio. Returns status byte read
 *              during transfer of command strobe. Validation of provided
 *              is not done. Function assumes chip is ready.
 *
 * input parameters
 *
 * @param       radio_id - which radio IC to communicate to
 * @param       cmd - command strobe
 *
 * output parameters
 *
 * @return      status byte
 */
rfStatus_t trxSpiCmdStrobe(uint8_t radio_id, uint8_t cmd)
{
    uint32_t rc;
	
	cs_low(radio_id);
	
	while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) & GPIO_PIN_0){};
	// do we need to flush the rx buffer first?
	
	while(SSIBusy(SSI1_BASE));
	SSIDataPut(SSI1_BASE, cmd);
	while(SSIBusy(SSI1_BASE));
	SSIDataGet(SSI1_BASE, &rc);
	
	
	cs_high(radio_id);
  
    return(rc);
}

/*******************************************************************************
 * @fn          trxReadWriteBurstSingle
 *
 * @brief       When the address byte is sent to the SPI slave, the next byte
 *              communicated is the data to be written or read. The address
 *              byte that holds information about read/write -and single/
 *              burst-access is provided to this function.
 *
 *              Depending on these two bits this function will write len bytes to
 *              the radio in burst mode or read len bytes from the radio in burst
 *              mode if the burst bit is set. If the burst bit is not set, only
 *              one data byte is communicated.
 *
 *              NOTE: This function is used in the following way:
 *
 *              TRXEM_SPI_BEGIN();
 *              while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
 *              ...[Depending on type of register access]
 *              trxReadWriteBurstSingle(uint8_t addr,uint8_t *pData,uint16_t len);
 *              TRXEM_SPI_END();
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
static void trxReadWriteBurstSingle(uint8_t addr, uint8_t *pData, uint16_t len)
{
	uint16_t i;
	uint32_t d,t;
	/* Communicate len number of bytes: if RX - the procedure sends 0x00 to push bytes from slave*/
  if(addr&RADIO_READ_ACCESS)
  {
    if(addr&RADIO_BURST_ACCESS)
    {
      for (i = 0; i < len; i++)
      {
		  SSIDataPut(SSI1_BASE, 0);
		  while(SSIBusy(SSI1_BASE));
		  SSIDataGet(SSI1_BASE, &t); //(uint32_t *)pData);
		  *pData++ = t;
          ////TRXEM_SPI_TX(0);            /* Possible to combining read and write as one access type */
          ////TRXEM_SPI_WAIT_DONE();
          ////*pData = TRXEM_SPI_RX();     /* Store pData from last pData RX */
          //pData++;
      }
    }
    else
    {
      SSIDataPut(SSI1_BASE, 0);
      while(SSIBusy(SSI1_BASE)){};
      SSIDataGet(SSI1_BASE, &t);
	  *pData = (uint8_t)t;
      //TRXEM_SPI_TX(0);
      //TRXEM_SPI_WAIT_DONE();
      //*pData = TRXEM_SPI_RX();
    }
  }
  else
  {
    if(addr&RADIO_BURST_ACCESS)
    {
      /* Communicate len number of bytes: if TX - the procedure doesn't overwrite pData */
      for (i = 0; i < len; i++)
      {
		SSIDataPut(SSI1_BASE, (uint32_t)*pData);
        while(SSIBusy(SSI1_BASE)){};
		SSIDataGet(SSI1_BASE, &d);
        //TRXEM_SPI_TX(*pData);
        //TRXEM_SPI_WAIT_DONE();
        pData++;
      }
    }
    else
    {
	   SSIDataPut(SSI1_BASE, (uint32_t)*pData);
       while(SSIBusy(SSI1_BASE)){};
	   SSIDataGet(SSI1_BASE, &d);
      //TRXEM_SPI_TX(*pData);
      //TRXEM_SPI_WAIT_DONE();
    }
  }
  return;
}