
#include "cc1125.h"
#include "cc112x_spi.h"
#include "inttypes.h"

#define CC_XO_FREQ 38400000


/*******************************************************************************
*   @fn         radio_set_wr_f
*
*   @brief      Sets the power to the requested floating point value in dBm. Will
*               overwrite the input value with the actual value. Call only when
*               in 'idle' state
*
*   @param      radio_id - select the radio to use
*               *pwr - pointer to the requested power (dBm)
*               *reg_value - used for reading back the register value, useful for 
*                            cal'ing the output
*
*   @return     0 - changed sucessfully; 1 - error, freq not set
*/
uint8_t radio_set_pwr_f(uint8_t radio_id, double *pwr, uint8_t *reg_value){
   
   uint8_t div;
   if ((*pwr < -11) || (*pwr > 15))      
      return 1;
   
   double p;
   p = (*pwr+1)/2-18;
   
   uint32_t pwr_reg = (int32_t)p;
   
   if (pwr_reg < 3)
      pwr_reg = 3;
   pwr_reg = pwr_reg & 0x3F;
   
   // work back to calculate the actual freq
   p = (2*(pwr_reg+18))-1;
   
   *pwr = p;
   *reg_value = pwr_reg;
   
   //now write to CC112X
   cc112xSpiWriteReg(radio_id, CC112X_PA_CFG2, (1<<6) | pwr_reg, 1);   
   
   return 0;  
   
} 

/*******************************************************************************
*   @fn         radio_set_freq_f
*
*   @brief      Sets the frequency to the requested floating point value. Will
*               overwrite the input value with the actual value. Call only when
*               in 'idle' state
*
*   @param      radio_id - select the radio to use
*               *freq - pointer to the requested frequency
*
*   @return     0 - changed sucessfully; 1 - error, freq not set
*/
uint8_t radio_set_freq_f(uint8_t radio_id, double *freq){
   
   uint8_t div;
   if ((*freq >= 136.7) && (*freq <= 160))
      div = 24;
   else if ((*freq >= 164) && (*freq <= 192))
      div = 20;
   else if ((*freq >= 205) && (*freq <= 240))
      div = 16;
   else if ((*freq >= 273.3) && (*freq <= 320))
      div = 12;
   else if ((*freq >= 410) && (*freq <= 480))
      div = 8;
   else if ((*freq >= 820) && (*freq <= 960))
      div = 4;
   else
      return 1;
   
   double f;
   f = div * ((*freq)*1000000) * 65536 / CC_XO_FREQ;
   
   uint32_t freq_reg = (int32_t)f;
   
   // work back to calculate the actual freq
   f = freq_reg*CC_XO_FREQ;
   f = f / div;
   f = f / 65536;
   
   *freq = f/1000000;
   
   
   //now write to CC112X
   cc112xSpiWriteReg(radio_id, CC112X_FREQ0, freq_reg & 0xFF, 1);
   cc112xSpiWriteReg(radio_id, CC112X_FREQ1, (freq_reg >> 8) & 0xFF, 1);
   cc112xSpiWriteReg(radio_id, CC112X_FREQ2, (freq_reg >> 16) & 0xFF, 1);
   cc112xSpiWriteReg(radio_id, CC112X_FS_CFG, 0x10 | div , 1);
   
   
   return 0;  
   
} 


/*******************************************************************************
*   @fn         radio_reset_config
*
*   @brief      Resets the requested radio and writes the default config for that
*               radio
*
*   @param      none
*
*   @return     none
*/
void radio_reset_config(uint8_t radio_id, registerSetting_t *cfg, uint16_t len){
	
	trxSpiCmdStrobe(radio_id, CC112X_SRES);
	uint8_t writeByte;
	for(uint16_t i = 0; i < len; i++) {
        writeByte = cfg[i].data;
        cc112xSpiWriteReg(radio_id, cfg[i].addr, &writeByte, 1);
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
void manualCalibration(uint8_t radio_id) {

    uint8_t original_fs_cal2;
    uint8_t calResults_for_vcdac_start_high[3];
    uint8_t calResults_for_vcdac_start_mid[3];
    uint8_t marcstate;
    uint8_t writeByte;

    // 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(radio_id, CC112X_FS_VCO2, &writeByte, 1);

    // 2) Start with high VCDAC (original VCDAC_START + 2):
    cc112xSpiReadReg(radio_id, CC112X_FS_CAL2, &original_fs_cal2, 1);
    writeByte = original_fs_cal2 + VCDAC_START_OFFSET;
    cc112xSpiWriteReg(radio_id, CC112X_FS_CAL2, &writeByte, 1);

    // 3) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)
    trxSpiCmdStrobe(radio_id, CC112X_SCAL);

    do {
        cc112xSpiReadReg(radio_id, CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);

    // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with 
    //    high VCDAC_START value
    cc112xSpiReadReg(radio_id, CC112X_FS_VCO2,
                     &calResults_for_vcdac_start_high[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(radio_id, CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_high[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(radio_id, CC112X_FS_CHP,
                     &calResults_for_vcdac_start_high[FS_CHP_INDEX], 1);

    // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(radio_id, CC112X_FS_VCO2, &writeByte, 1);

    // 6) Continue with mid VCDAC (original VCDAC_START):
    writeByte = original_fs_cal2;
    cc112xSpiWriteReg(radio_id, CC112X_FS_CAL2, &writeByte, 1);

    // 7) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)
    trxSpiCmdStrobe(radio_id, CC112X_SCAL);

    do {
        cc112xSpiReadReg(radio_id, CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);

    // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained 
    //    with mid VCDAC_START value
    cc112xSpiReadReg(radio_id, CC112X_FS_VCO2, 
                     &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(radio_id, CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(radio_id, CC112X_FS_CHP,
                     &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);

    // 9) Write back highest FS_VCO2 and corresponding FS_VCO
    //    and FS_CHP result
	
    if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] >
        calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
        writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_CHP, &writeByte, 1);
    } else {
        writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_CHP, &writeByte, 1);
    }
}