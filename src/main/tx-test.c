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
	
	/*
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinConfigure(GPIO_PF2_SSI1CLK);
    GPIOPinConfigure(GPIO_PF3_SSI1FSS);
    GPIOPinConfigure(GPIO_PF0_SSI1RX);
    GPIOPinConfigure(GPIO_PF1_SSI1TX);
	GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 |
                   GPIO_PIN_1);
	SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);
	
	
	SSIEnable(SSI1_BASE);
	
	while(SSIDataGetNonBlocking(SSI1_BASE, &pui32DataRx[0]))
    {
    }
	
	pui32DataTx[0] = 's';
    pui32DataTx[1] = 'p';
    pui32DataTx[2] = 'i';
	
	*/
	

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

	radio_reset_config(radio_id, preferredSettings, sizeof(preferredSettings)/sizeof(registerSetting_t));
	/*
	trxSpiCmdStrobe(radio_id, CC112X_SRES);
	uint8_t writeByte;
	for(uint16_t i = 0; i < sizeof(preferredSettings)/sizeof(registerSetting_t); i++) {
        writeByte = preferredSettings[i].data;
        cc112xSpiWriteReg(radio_id, preferredSettings[i].addr, &writeByte, 1);
    }
	*/
	
	
	UART_puts(UART_PC104_HEADER, "DONE");
	for(ui32Loop = 0; ui32Loop < 3000; ui32Loop++) {};
	
	//////// Calibrate radio according to errata
    manualCalibration(radio_id);

	
	for(ui32Loop = 0; ui32Loop < 3000; ui32Loop++) {};
	UART_puts(UART_PC104_HEADER, "CAL'd");
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
        // Turn on the LED.
        LED_off(LED_B);


		/* On period */
		for(ui32Loop = 0; ui32Loop < 300000; ui32Loop++) {};

		LED_off(LED_B);

        // Delay for a bit.
        for(ui32Loop = 0; ui32Loop < 300000; ui32Loop++) {};
		
		uint8_t ui32Index;
		/*for(ui32Index = 0; ui32Index < 3; ui32Index++)
			SSIDataPut(SSI1_BASE, pui32DataTx[ui32Index]);
		while(SSIBusy(SSI1_BASE));*/
		
		
		
		
		// Write packet to TX FIFO
        cc112xSpiWriteTxFifo(radio_id, buff, sizeof(buff));

        // Strobe TX to send packet
        trxSpiCmdStrobe(radio_id, CC112X_STX);
		
		
		// TODO: wait for packet to be sent  (the example uses interrupts)
		
		
    }
}


