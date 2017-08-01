/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

#include "../cc1125/cc1125.h"


#define CONFIG_1
#include "cc1120_rx_sniff_mode_reg_config.h"







//! A very simple example that blinks the on-board LED.

uint32_t pui32DataTx[10];
uint32_t pui32DataRx[10];

#define PACKETLEN 100

int main(void)
{

    volatile uint32_t ui32Loop;

    Board_init();
	
	uint8_t rxBuffer[128] = {0};	

	UART_init(UART_PC104_HEADER, 500000);
  UART_putc(UART_PC104_HEADER, '\r');
	UART_putc(UART_PC104_HEADER, '\n');
  for(ui32Loop = 0; ui32Loop < 300000; ui32Loop++) {};

  UART_putc(UART_PC104_HEADER, 'M');
	
	trxRfSpiInterfaceInit(0);
	
	
	uint8_t radio_id = RADIO_TX;
	
	//////// write config ///////

    // Reset radio
    // Write registers to radio

	radio_reset_config(RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));
	radio_reset_config(RADIO_RX, preferredSettings, sizeof(preferredSettings)/sizeof(registerSetting_t));
	/*
	trxSpiCmdStrobe(radio_id, CC112X_SRES);
	uint8_t writeByte;
	for(uint16_t i = 0; i < sizeof(preferredSettings)/sizeof(registerSetting_t); i++) {
        writeByte = preferredSettings[i].data;
        cc112xSpiWriteReg(radio_id, preferredSettings[i].addr, &writeByte, 1);
    }
	*/
	
	
	//UART_puts(UART_PC104_HEADER, "DONE");
	for(ui32Loop = 0; ui32Loop < 3000; ui32Loop++) {};
	
	//////// Calibrate radio according to errata
    manualCalibration(RADIO_TX);
    manualCalibration(RADIO_RX);

	
	for(ui32Loop = 0; ui32Loop < 3000; ui32Loop++) {};
	//UART_puts(UART_PC104_HEADER, "CAL'd");
	for(ui32Loop = 0; ui32Loop < 30000; ui32Loop++) {};
	
	/////// the packet
	uint8_t buff[PACKETLEN+1];
	buff[0] = PACKETLEN;
	 // Fill rest of buffer with random bytes
    for(uint8_t i = 1; i < (PACKETLEN + 1); i++) {
        buff[i] = (uint8_t)i; //rand();
    }
	
	
    // Loop forever.
    while(1)
    {
 		
		
		//// Write packet to TX FIFO
        //cc112xSpiWriteTxFifo(RADIO_TX, buff, sizeof(buff));

		trxSpiCmdStrobe(RADIO_RX, CC112X_SRX);
		
        // Strobe TX to send packet
        trxSpiCmdStrobe(RADIO_TX, CC112X_STX);
		
		while(1){
			WDT_kick();
			// Delay for a bit.
			for(ui32Loop = 0; ui32Loop < 300000; ui32Loop++) {};

		}
		
	/*	
		// Wait for RX. If we dont get anything the WDT will kick 
		// in and we can try again later
		UART_puts(UART_PC104_HEADER, "waiting... ");
		// Wait for falling edge of GPIO0
		while(pollRadioGPIO0(RADIO_RX) == 0){};
		while(pollRadioGPIO0(RADIO_RX) > 0){};
		UART_puts(UART_PC104_HEADER, " flag high ");
		
		// Read number of bytes in RX FIFO
		uint8_t rxBytes, marcState;
        cc112xSpiReadReg(RADIO_RX, CC112X_NUM_RXBYTES, &rxBytes, 1);
				
		// Check that we have bytes in FIFO
		if(rxBytes != 0) {
			UART_puts(UART_PC104_HEADER, "Got some bytes: ");
			// Read MARCSTATE to check for RX FIFO error
			cc112xSpiReadReg(RADIO_RX, CC112X_MARCSTATE, &marcState, 1);

			// Mask out MARCSTATE bits and check if we have a RX FIFO error
			if((marcState & 0x1F) == 0x11) { //RX_FIFO_ERROR) {
				UART_puts(UART_PC104_HEADER, "RX FIFO error :(\n");
				// Flush RX FIFO
				trxSpiCmdStrobe(RADIO_RX, CC112X_SFRX);
			} else {

				// Read n bytes from RX FIFO
				cc112xSpiReadRxFifo(RADIO_RX, rxBuffer, rxBytes);
				

				for (uint8_t i = 0; i < rxBytes; i++)
					UART_puts(UART_PC104_HEADER, (char *)&rxBuffer[i]);
				
				// Check CRC ok (CRC_OK: bit7 in second status byte)
				// This assumes status bytes are appended in RX_FIFO
				// (PKT_CFG1.APPEND_STATUS = 1)
				// If CRC is disabled the CRC_OK field will read 1
				//if(rxBuffer[rxBytes - 1] & 0x80) {
				//
				//	// Update packet counter
				//	packetCounter++;
				//}
			}
		}
		else
			UART_puts(UART_PC104_HEADER, "No bytes.:(");

*/
    }
}

