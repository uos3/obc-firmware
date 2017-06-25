/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"


#define CONFIG_1


#include "cc1120_rx_sniff_mode_reg_config.h"



//! A very simple example that blinks the on-board LED.

static void manualCalibration(void);

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
	
	UART_init(UART_PC104_HEADER, 9600);
  UART_putc(UART_PC104_HEADER, '\r');
	UART_putc(UART_PC104_HEADER, '\n');
  for(ui32Loop = 0; ui32Loop < 300000; ui32Loop++) {};

  UART_putc(UART_PC104_HEADER, 'M');
	
	trxRfSpiInterfaceInit(0);
	
	//////// write config ///////
	uint8_t writeByte;
    // Reset radio
    trxSpiCmdStrobe(CC112X_SRES);
    // Write registers to radio
    for(uint16_t i = 0; i < (sizeof(preferredSettings)/sizeof(registerSetting_t)); i++) {
//UART_putc(UART_PC104_HEADER, (sizeof(preferredSettings)/sizeof(registerSetting_t)));
        writeByte = preferredSettings[i].data;
UART_putc(UART_PC104_HEADER, 'K');
        cc112xSpiWriteReg(preferredSettings[i].addr, &writeByte, 1);
UART_putc(UART_PC104_HEADER, 'J');
    }
	
	//////// Calibrate radio according to errata
    manualCalibration();
	
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
        cc112xSpiWriteTxFifo(buff, sizeof(buff));

        // Strobe TX to send packet
        trxSpiCmdStrobe(CC112X_STX);
		
		
		// TODO: wait for packet to be sent  (the example uses interrupts)
		
		
    }
}


/*******************************************************************************
*   @fn         manualCalibration
*
*   @brief      Calibrates radio according to CC112x errata
*
*   @param      none
*
*   @return     none
*/
#define VCDAC_START_OFFSET 2
#define FS_VCO2_INDEX 0
#define FS_VCO4_INDEX 1
#define FS_CHP_INDEX 2
static void manualCalibration(void) {

    uint8_t original_fs_cal2;
    uint8_t calResults_for_vcdac_start_high[3];
    uint8_t calResults_for_vcdac_start_mid[3];
    uint8_t marcstate;
    uint8_t writeByte;

    // 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);

    // 2) Start with high VCDAC (original VCDAC_START + 2):
    cc112xSpiReadReg(CC112X_FS_CAL2, &original_fs_cal2, 1);
    writeByte = original_fs_cal2 + VCDAC_START_OFFSET;
    cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);

    // 3) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);

    do {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);

    // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with 
    //    high VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2,
                     &calResults_for_vcdac_start_high[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_high[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP,
                     &calResults_for_vcdac_start_high[FS_CHP_INDEX], 1);

    // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);

    // 6) Continue with mid VCDAC (original VCDAC_START):
    writeByte = original_fs_cal2;
    cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);

    // 7) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);

    do {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);

    // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained 
    //    with mid VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2, 
                     &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP,
                     &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);

    // 9) Write back highest FS_VCO2 and corresponding FS_VCO
    //    and FS_CHP result
    if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] >
        calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
        writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    } else {
        writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    }
}