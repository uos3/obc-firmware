
#include "cc1125.h"
#include "cc112x_spi.h"
#include "inttypes.h"

#include "../firmware.h"

#define CC_XO_FREQ 38400000

#include <stdlib.h>
#include <stdio.h>



/* Pins: GPIO0_RADIO_RX / GPIO0_RADIO_TX */
bool cc1125_pollGPIO(uint8_t gpio_pin)
{
  return GPIO_read(gpio_pin);
}

/*******************************************************************************
*   @fn         radio_set_rxbw_param
*
*   @brief      Sets the RX bandwidth. Will overwrite the input value with the
*               actual value. Call only when in 'idle' state.
*
*   @param      radio_id - select the radio to use
*               *rxbw - pointer to the requested RX bandwidth (Hz)
*
*   @return     0 - changed sucessfully; 1 - error, freq not set
*/
uint8_t radio_set_rxbw_param(uint8_t radio_id, uint32_t *rxbw){ //, uint32_t *symrate){
   
   
   uint8_t d_fact[4]
   uint8_t i,sm;
   uint32_t err, sm_err;
   int32_t bw[4];
   
   // first try with cic = 20
   d_fact[0] = (uint32_t)CC_XO_FREQ/(*rxbw)/8/20;
   d_fact[1] = d_fact[0]+1;
   
   // also try with cic = 32
   d_fact[2] = (uint32_t)CC_XO_FREQ/(*rxbw)/8/32;
   d_fact[3] = d_fact[2]+1;
   
   //set limits
   for (i = 0; i < 4; i++){
      if (d_fact[i] < 1)
         d_fact[i] = 1;
      if (d_fact[i] > 44)
         d_fact[i] = 44;
   }
   
   //work back to get the actual BWs   
   bw[0] = (uint32_t)CC_XO_FREQ/d_fact[0]/8/20;
   bw[1] = (uint32_t)CC_XO_FREQ/d_fact[1]/8/20;
   bw[2] = (uint32_t)CC_XO_FREQ/d_fact[2]/8/32;
   bw[3] = (uint32_t)CC_XO_FREQ/d_fact[3]/8/32;
     
   
   sm_err = 1<<30;
   sm = 0;
   for (i = 0; i < 4; i++){
      err = abs(bw[0]-(int32_t)(*rxbw));
      if (err < sm_err){
         sm_err = err;
         sm = i;
      }
   }
   
   uint8_t reg = 0;
   
   // set ADC_CIC_DECFACT to 20 (0) or 32 (1)
   if (sm <= 1)
      reg = (0<<6);
   else
      reg = (1<<6);
   
   reg |= (d_fact[sm] & 0x3F);
   
   *rxbw = bw[sm];
   
   return 0;   
   
}


/*******************************************************************************
*   @fn         radio_set_fsk_param
*
*   @brief      Sets the FSK symbol rate and deviation to the requested values.
*               Will overwrite the input value with the actual value. Call only 
*               when in 'idle' state
*
*   @param      radio_id - select the radio to use
*               *symbol_rate - pointer to the requested symbol rate (/sec)
*               *deviation - pointer to the requested deviation (Hz)
*
*   @return     0 - changed sucessfully; 1 - error, freq not set
*/
uint8_t radio_set_fsk_param(uint8_t radio_id, uint32_t *symbol_rate, uint32_t *deviation){
   
   // deviation
   uint64_t d,d1;
   uint8_t m,e;
   
   d = ((uint64_t)(*deviation) * (1<<24))/CC_XO_FREQ;
   
   d1 = d >> 8;
   
   if (d1 < 2){
      e = 0;
      m = (uint8_t)(((uint64_t)(*deviation) * (1<<23))/CC_XO_FREQ);
   }else{
      e = 0;
      while(d1){
         d1 = d1 >> 1;
         e++;
      }
      e--;
      m = (uint8_t)(((((uint64_t)(*deviation) * (1<<24))/CC_XO_FREQ) >> e) - 256); 
   }
   if (e > 7)
      return 1;
   
   //symbol rate
   uint64_t s,s1;
   uint32_t ms;
   uint8_t es;
   
   s = ((uint64_t)(*symbol_rate) * ((uint64_t)1<<39))/((uint64_t)CC_XO_FREQ);
   s1 = s >> 20;
   
   if (s1 < 2){
      es = 0;
      ms = (uint32_t)(((uint64_t)(*symbol_rate) * ((uint64_t)1<<38))/((uint64_t)CC_XO_FREQ));
   }
   else{
      es = 0;
      while(s1){
         s1 = s1 >> 1;
         es++;
      }
      es--;
      ms = (uint32_t)(((((uint64_t)(*symbol_rate) * ((uint64_t)1<<39))/((uint64_t)CC_XO_FREQ)) >> es) - ((uint64_t)1<<20)); 
   }
   if (es > 15)
      return 1;
  
   
   
   // work back to calculate the actual values
   if (e == 0)
      *deviation = (uint32_t)(((uint64_t)CC_XO_FREQ * m ) >> 23);
   else
      *deviation = (uint32_t)((CC_XO_FREQ * (256+(uint64_t)m) ) >> (24-e));
   
   if (es == 0)
      *symbol_rate = (uint32_t)((((uint64_t)CC_XO_FREQ) * ms ) >> 38);
   else
      *symbol_rate = (uint32_t)((((uint64_t)CC_XO_FREQ) * ((1<<20)+(uint64_t)ms) ) >> (39-es));
   

   uint8_t writebyte;
   writebyte = (uint8_t)(ms & 0xFF);
   cc112xSpiWriteReg(radio_id, CC112X_SYMBOL_RATE0, &writebyte);
   writebyte = (uint8_t)((ms>>8) & 0xFF);
   cc112xSpiWriteReg(radio_id, CC112X_SYMBOL_RATE1, &writebyte);
   writebyte = (uint8_t)(((ms>>16) & 0x0F) | ((es<<4) & 0xF0));
   cc112xSpiWriteReg(radio_id, CC112X_SYMBOL_RATE2, &writebyte);
   
   cc112xSpiWriteReg(radio_id, CC112X_DEVIATION_M , &m);
   writebyte = (uint8_t)((e & 0x07) | (0 << 3) |  (0<<6));  // (0<<3) - FSK mode
   cc112xSpiWriteReg(radio_id, CC112X_MODCFG_DEV_E , &writebyte);

      
   /*
   char buff[30];
   snprintf(buff, 30, "\n\n%02x  %02x  %02x\n\n",((ms>>16) & 0x0F) | ((es<<4) & 0xF0), (ms>>8) & 0xFF,  (ms & 0xFF) );
   UART_puts(UART_CAM_HEADER, buff);
   snprintf(buff, 30, "\n\n%02x  %02x \n\n",(uint8_t)((e & 0x07) | (0 << 3) |  (0<<6)), m);
   UART_puts(UART_CAM_HEADER, buff);
   */
   
   return 0;  
   
} 


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
   
   if ((*pwr < -11) || (*pwr > 15))      
      return 1;
   
   double p;
   p = (2*(*pwr+18))-1;
   
   uint32_t pwr_reg = (uint32_t)p;
   
   if (pwr_reg < 3)
      pwr_reg = 3;
   pwr_reg = pwr_reg & 0x3F;
   
   // work back to calculate the actual freq
   p = ((double)pwr_reg+1)/2-18;
   
   *pwr = p;
   *reg_value = (uint8_t)pwr_reg;
   
   //now write to CC112X
   //cc112xSpiWriteReg(radio_id, CC112X_PA_CFG2, (1<<6) | pwr_reg, 1);
   uint8_t writebyte;
   writebyte = (uint8_t)((1<<6) | pwr_reg);
   cc112xSpiWriteReg(radio_id, CC112X_PA_CFG2, &writebyte);   
   
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
   
   uint8_t div, bandsel;
   if ((*freq >= 136.7) && (*freq <= 160)){
      div = 24;
      bandsel = 11; }
   else if ((*freq >= 164) && (*freq <= 192)){
      div = 20;
      bandsel = 10; }
   else if ((*freq >= 205) && (*freq <= 240)){
      div = 16;
      bandsel = 8; }
   else if ((*freq >= 273.3) && (*freq <= 320)){
      div = 12;
      bandsel = 6; }
   else if ((*freq >= 410) && (*freq <= 480)){
      div = 8;
      bandsel = 4; }
   else if ((*freq >= 820) && (*freq <= 960)){
      div = 4;
      bandsel = 2; }
   else
      return 1;
   
   double f;
   f = div * ((*freq)*1000000) * 65536 / CC_XO_FREQ;
   
   uint32_t freq_reg = (uint32_t)f;
   
   // work back to calculate the actual freq
   f = (double)freq_reg*CC_XO_FREQ;
   f = f / div;
   f = f / 65536;
   
   *freq = f/1000000;
   
   
   //now write to CC112X
   uint8_t writebyte;
   writebyte = freq_reg & 0xFF;
   cc112xSpiWriteReg(radio_id, CC112X_FREQ0, &writebyte);
   writebyte = (freq_reg >> 8) & 0xFF;
   cc112xSpiWriteReg(radio_id, CC112X_FREQ1, &writebyte);
   writebyte = (freq_reg >> 16) & 0xFF;
   cc112xSpiWriteReg(radio_id, CC112X_FREQ2, &writebyte);
   writebyte = 0x10 | bandsel;
   cc112xSpiWriteReg(radio_id, CC112X_FS_CFG, &writebyte);
   
   
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
void radio_reset_config(uint8_t radio_id, const registerSetting_t *cfg, uint16_t len){
	
  SPI_cmdstrobe(radio_id, CC112X_SRES);
	uint8_t writeByte;
	for(uint16_t i = 0; i < len; i++) {
        writeByte = cfg[i].data;
        cc112xSpiWriteReg(radio_id, cfg[i].addr, &writeByte);
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
    cc112xSpiWriteReg(radio_id, CC112X_FS_VCO2, &writeByte);

    // 2) Start with high VCDAC (original VCDAC_START + 2):
    cc112xSpiReadReg(radio_id, CC112X_FS_CAL2, &original_fs_cal2);
    writeByte = (uint8_t)(original_fs_cal2 + VCDAC_START_OFFSET);
    cc112xSpiWriteReg(radio_id, CC112X_FS_CAL2, &writeByte);

    // 3) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)
    SPI_cmdstrobe(radio_id, CC112X_SCAL);

    do {
        cc112xSpiReadReg(radio_id, CC112X_MARCSTATE, &marcstate);
    } while (marcstate != 0x41);

    // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with 
    //    high VCDAC_START value
    cc112xSpiReadReg(radio_id, CC112X_FS_VCO2,
                     &calResults_for_vcdac_start_high[FS_VCO2_INDEX]);
    cc112xSpiReadReg(radio_id, CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_high[FS_VCO4_INDEX]);
    cc112xSpiReadReg(radio_id, CC112X_FS_CHP,
                     &calResults_for_vcdac_start_high[FS_CHP_INDEX]);

    // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(radio_id, CC112X_FS_VCO2, &writeByte);

    // 6) Continue with mid VCDAC (original VCDAC_START):
    writeByte = original_fs_cal2;
    cc112xSpiWriteReg(radio_id, CC112X_FS_CAL2, &writeByte);

    // 7) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)

    SPI_cmdstrobe(radio_id, CC112X_SCAL);

    do {
        cc112xSpiReadReg(radio_id, CC112X_MARCSTATE, &marcstate);
    } while (marcstate != 0x41);

    // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained 
    //    with mid VCDAC_START value
    cc112xSpiReadReg(radio_id, CC112X_FS_VCO2, 
                     &calResults_for_vcdac_start_mid[FS_VCO2_INDEX]);
    cc112xSpiReadReg(radio_id, CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_mid[FS_VCO4_INDEX]);
    cc112xSpiReadReg(radio_id, CC112X_FS_CHP,
                     &calResults_for_vcdac_start_mid[FS_CHP_INDEX]);

    // 9) Write back highest FS_VCO2 and corresponding FS_VCO
    //    and FS_CHP result
	
    if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] >
        calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
        writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_VCO2, &writeByte);
        writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_VCO4, &writeByte);
        writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_CHP, &writeByte);
    } else {
        writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_VCO2, &writeByte);
        writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_VCO4, &writeByte);
        writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
        cc112xSpiWriteReg(radio_id, CC112X_FS_CHP, &writeByte);
    }
}