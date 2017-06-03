//******************************************************************************
//! @file       cc1120_rx_sniff_mode_tx.c
//! @brief      This program demonstrates Rx Sniff Mode for the 1.2 kbps,
//              50 kHz RX filter BW setting available in SmartRf Studio.
//              The transmitter sends a packet every time a button is pushed and
//              the receiver implements RX Sniff Mode to reduce the current
//              consumption.
//              DN511 (http://www.ti.com/lit/swra428) explains how the register
//              settings are found.
//              Note! It is important that RX and TX configurations matches:
//              RX (Config 1) - TX (Config 1)
//              RX (Config 2) - TX (Config 2 and 3)
//              RX (Config 3) - TX (Config 2 and 3)
//
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//      Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//      Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//      Neither the name of Texas Instruments Incorporated nor the names of
//      its contributors may be used to endorse or promote products derived
//      from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//*****************************************************************************/


/*******************************************************************************
* INCLUDES
*/
#include "msp430.h"
#include "lcd_dogm128_6.h"
#include "hal_spi_rf_trxeb.h"
#include "cc112x_spi.h"
#include "stdlib.h"
#include "bsp.h"
#include "bsp_key.h"
#include "io_pin_int.h"
#include "bsp_led.h"
#include "cc1120_rx_sniff_mode_reg_config.h"


/*******************************************************************************
* DEFINES
*/
#define ISR_ACTION_REQUIRED     1
#define ISR_IDLE                0
#define PKTLEN                  30 // 1 < PKTLEN < 126

#define GPIO3                   0x04
#define GPIO2                   0x08
#define GPIO0                   0x80


/*******************************************************************************
* LOCAL VARIABLES
*/
static uint8  packetSemaphore;
static uint32 packetCounter = 0;


/*******************************************************************************
* STATIC FUNCTIONS
*/
static void initMCU(void);
static void registerConfig(void);
static void manualCalibration(void);
static void runTX(void);
static void createPacket(uint8 randBuffer[]);
static void radioTxISR(void);
static void updateLcd(void);


/*******************************************************************************
*   @fn         main
*
*   @brief      Runs the main routine.
*
*   @param      none
*
*   @return     none
*/
void main(void) {

    // Initialize MCU and peripherals
    initMCU();

    // Write radio registers
    registerConfig();

    // Enter runTX, never coming back
    runTX();
}


/*******************************************************************************
*   @fn         runTX
*
*   @brief      Transmits a packet every time a button is pushed. A packet
*               counter is incremented for each packet sent and the LCD is
*               updated
*
*   @param      none
*
*   @return     none
*/
static void runTX(void) {

    // Initialize packet buffer of size PKTLEN + 1
    uint8 txBuffer[PKTLEN + 1];

    // Connect ISR function to GPIO0
    ioPinIntRegister(IO_PIN_PORT_1, GPIO0, &radioTxISR);

    // Interrupt on falling edge
    ioPinIntTypeSet(IO_PIN_PORT_1, GPIO0, IO_PIN_FALLING_EDGE);

    // Clear ISR flag
    ioPinIntClear(IO_PIN_PORT_1, GPIO0);

    // Enable interrupt
    ioPinIntEnable(IO_PIN_PORT_1, GPIO0);

    // Update LCD
    updateLcd();

    // Calibrate radio according to errata
    manualCalibration();

    // Infinite loop
    while(TRUE) {

        // Wait for button push
        while(!bspKeyPushed(BSP_KEY_ALL));

        // Create a random packet with PKTLEN + 2 byte packet counter + 
        // n x random bytes
        createPacket(txBuffer);

        // Write packet to TX FIFO
        cc112xSpiWriteTxFifo(txBuffer, sizeof(txBuffer));

        // Strobe TX to send packet
        trxSpiCmdStrobe(CC112X_STX);

        // Wait for packet to be sent
        while(packetSemaphore != ISR_ACTION_REQUIRED);

        // Clear semaphore flag
        packetSemaphore = ISR_IDLE;

        // Update LCD
        updateLcd();
    }
}


/*******************************************************************************
*   @fn         radioTxISR
*
*   @brief      ISR for packet handling in TX. Sets packet semaphore
*               and clears interrupt flag
*
*   @param      none
*
*   @return     none
*/
static void radioTxISR(void) {

    // Set packet semaphore
    packetSemaphore = ISR_ACTION_REQUIRED;

    // Clear ISR flag
    ioPinIntClear(IO_PIN_PORT_1, GPIO0);
}


/*******************************************************************************
*   @fn         initMCU
*
*   @brief      Initialize MCU and board peripherals
*
*   @param      none
*
*   @return     none
*/
static void initMCU(void) {

    // Init clocks and I/O
    bspInit(BSP_SYS_CLK_8MHZ);

    // Init LEDs
    bspLedInit();

    // Init buttons
    bspKeyInit(BSP_KEY_MODE_POLL);

    // Initialize SPI interface to LCD (shared with SPI flash)
    bspIoSpiInit(BSP_FLASH_LCD_SPI, BSP_FLASH_LCD_SPI_SPD);

    // Init LCD
    lcdInit();

    // Instantiate transceiver RF SPI interface to SCLK ~ 4 MHz
    // Input parameter is clockDivider
    // SCLK frequency = SMCLK/clockDivider
    trxRfSpiInterfaceInit(2);

    // Enable global interrupt
    _BIS_SR(GIE);
}


/*******************************************************************************
*   @fn         registerConfig
*
*   @brief      Write register settings as given by SmartRF Studio found in
*               cc1120_rx_sniff_mode_reg_config.h
*
*   @param      none
*
*   @return     none
*/
static void registerConfig(void) {

    uint8 writeByte;

    // Reset radio
    trxSpiCmdStrobe(CC112X_SRES);

    // Write registers to radio
    for(uint16 i = 0;
        i < (sizeof(preferredSettings)/sizeof(registerSetting_t)); i++) {
        writeByte = preferredSettings[i].data;
        cc112xSpiWriteReg(preferredSettings[i].addr, &writeByte, 1);
    }
}


/*******************************************************************************
*   @fn         createPacket
*
*   @brief      This function is called before a packet is transmitted. It fills
*               the txBuffer with a packet consisting of a length byte, two
*               bytes packet counter and n random bytes
*
*               The packet format is as follows:
*               |--------------------------------------------------------------|
*               |           |           |           |         |       |        |
*               | pktLength | pktCount1 | pktCount0 | rndData |.......| rndData|
*               |           |           |           |         |       |        |
*               |--------------------------------------------------------------|
*                txBuffer[0] txBuffer[1] txBuffer[2]  ......... txBuffer[PKTLEN]
*
*   @param      Pointer to start of txBuffer
*
*   @return     none
*/
static void createPacket(uint8 txBuffer[]) {

    txBuffer[0] = PKTLEN;                           // Length byte
    txBuffer[1] = (uint8) (packetCounter >> 8);     // MSB of packetCounter
    txBuffer[2] = (uint8) packetCounter;            // LSB of packetCounter

    // Fill rest of buffer with random bytes
    for(uint8 i = 3; i < (PKTLEN + 1); i++) {
        txBuffer[i] = (uint8)rand();
    }
}


/*******************************************************************************
*   @fn         updateLcd
*
*   @brief      Updates LCD buffer and sends buffer to LCD module
*
*   @param      none
*
*   @return     none
*/
static void updateLcd(void) {

    // Update LDC buffer and send to screen
    lcdBufferClear(0);
    lcdBufferPrintString(0, "RX Sniff Mode", 0, eLcdPage0);
    lcdBufferSetHLine(0, 0, LCD_COLS - 1, 7);
    lcdBufferPrintString(0, "Sent packets:", 0, eLcdPage3);
    lcdBufferPrintInt(0, packetCounter++, 80, eLcdPage3);
    #ifdef CONFIG_1
        lcdBufferPrintString(0, "TX, config. 1", 0, eLcdPage7);
    #endif
    #ifdef CONFIG_2
        lcdBufferPrintString(0, "TX, config. 2 and 3", 0, eLcdPage7);
    #endif
    lcdBufferSetHLine(0, 0, LCD_COLS - 1, 55);
    lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
    lcdSendBuffer(0);
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

    uint8 original_fs_cal2;
    uint8 calResults_for_vcdac_start_high[3];
    uint8 calResults_for_vcdac_start_mid[3];
    uint8 marcstate;
    uint8 writeByte;

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