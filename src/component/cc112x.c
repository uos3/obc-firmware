//#include "../firmware.h"

// #include <string.h>
// #include <stdint.h>
// #include <stdbool.h>
// #include <stddef.h>
// #include <inttypes.h>
// #include <limits.h>
// #include "../board/spi.h"
#include <stdbool.h>
#include <inttypes.h>
#include "cc112x.h"
#include "cc112x_spi.h"
#include "../utility/debug.h"
// #include "cc112x_spi.h"
#if CC_XO_FREQ != 38400000
  #error CC112X Lib is currently hardcoded for CC_XO_FREQ of 38.4MHz
#endif

/* Pins: GPIO0_RADIO_RX / GPIO0_RADIO_TX */
bool cc1125_pollGPIO(uint8_t gpio_pin)
{
  return GPIO_read(gpio_pin);
}


void cc112x_reset(uint8_t spi_device_id)
{
  SPI_cmd(spi_device_id, CC112X_SRES);
}

void cc112x_powerdown(uint8_t spi_device_id)
{
  SPI_cmd(spi_device_id, CC112X_SPWD);
}

uint8_t cc112x_query_partnumber(uint8_t spi_device_id)
{
  uint8_t result;
  cc112xSpiReadReg(spi_device_id, (CC112X_PARTNUMBER), &result);
  return result;
}

uint8_t cc112x_query_num_txbytes(uint8_t spi_device_id)
{
  uint8_t result;
  cc112xSpiReadReg(spi_device_id, (CC112X_NUM_TXBYTES), &result);
  return result;
}

void cc112x_cfg_frequency(uint8_t spi_device_id, float freq) // cc112x_bandsel_option_t bandsel_option, uint32_t _frequency_word)
{

  uint8_t divisor, bandsel;
  if ((freq >= 136.7) && (freq <= 160)){
    divisor = 24;
    bandsel = 11; }
  else if ((freq >= 164) && (freq <= 192)){
    divisor = 20;
    bandsel = 10; }
  else if ((freq >= 205) && (freq <= 240)){
    divisor = 16;
    bandsel = 8; }
  else if ((freq >= 273.3) && (freq <= 320)){
    divisor = 12;
    bandsel = 6; }
  else if ((freq >= 410) && (freq <= 480)){
    divisor = 8;
    bandsel = 4; }
  else if ((freq >= 820) && (freq <= 960)){
    divisor = 4;
    bandsel = 2; }

  float f;
  f = divisor * ((freq)*1000000) * 65536 / CC_XO_FREQ;
  uint32_t freq_reg = (uint32_t)f;

   // work back to calculate the actual freq
   f = (float)freq_reg*CC_XO_FREQ;
   f = f / divisor;
   f = f / 65536;
   f = f/1000000;



  uint8_t val;

  //SPI_cmd(spi_device_id, CC112X_SIDLE);   //Set to idle

  val = (0x10 | (uint8_t)bandsel);
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_CFG), &val);

  val = (uint8_t)(freq_reg & 0xFF);
  // SPI_cmd(spi_device_id, 0x2F);
  // SPI_write8(spi_device_id, (CC112X_FREQ0 & 0xFF), &val);
  cc112xSpiWriteReg(spi_device_id, (CC112X_FREQ0), &val);

  // SPI_cmd(spi_device_id, 0x2F);
  // SPI_write8(spi_device_id, (CC112X_FREQ1 & 0xFF), &val);
  val = (uint8_t)((freq_reg >> 8) & 0xFF);
  cc112xSpiWriteReg(spi_device_id, (CC112X_FREQ1), &val);

  val = (uint8_t)((freq_reg >> 16) & 0xFF);
  // SPI_cmd(spi_device_id, 0x2F);
  // SPI_write8(spi_device_id, (CC112X_FREQ2 & 0xFF), &val);
  cc112xSpiWriteReg(spi_device_id, (CC112X_FREQ2), &val);

  uint32_t freqRead = 0;
  cc112xSpiReadReg(spi_device_id, (CC112X_FREQ0 & 0x7FFF), &val);
  freqRead |= val;
  cc112xSpiReadReg(spi_device_id, (CC112X_FREQ1 & 0x7FFF), &val);
  freqRead |= val<<8;
  cc112xSpiReadReg(spi_device_id, (CC112X_FREQ2 & 0x7FFF), &val);
  freqRead |= val<<16;

  char output[100];

  sprintf(output,"bandsel: %d,     div: %d    freq returned: %f     freq reg: %d\r\n", bandsel, divisor, f, freq_reg);
  UART_puts(UART_INTERFACE, output);
  sprintf(output,"Freq reg read  %d\r\n", freqRead);
  UART_puts(UART_INTERFACE, output);
}

void cc112x_cfg_power(uint8_t spi_device_id, int8_t power_dbm)
{
  uint8_t val;

  if(power_dbm >= -11 && power_dbm <= 15)
  {
    val = (uint8_t)(((2*(power_dbm+18))-1) & 0x3F);
    cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_PA_CFG2 & 0x7F), &val);
  }
}

void cc112x_cfg_preamble(uint8_t spi_device_id, cc112x_preamble_option_t preamble_option, cc112x_preamble_length_t preamble_length)
{
  uint8_t val;

  val = (uint8_t)(((preamble_length & 0x0F) << 2) | (preamble_option & 0x03));
  SPI_write8(spi_device_id, (uint8_t)(CC112X_PREAMBLE_CFG1 & 0x7F), &val);
}

void cc112x_cfg_syncword(uint8_t spi_device_id, cc112x_syncword_length_t syncword_length, cc112x_syncword_errorcheck_t syncword_errorcheck)
{
  uint8_t val;

  val = (uint8_t)(((syncword_length & 0x07) << 2) | (syncword_errorcheck & 0x03));
  SPI_write8(spi_device_id, (uint8_t)(CC112X_SYNC_CFG0 & 0x7F), &val);
}

void cc112x_cfg_packetlength(uint8_t spi_device_id, cc112x_packetlength_t packetlength_option, uint8_t fixedpacket_length)
{
  uint8_t val;

  val = (uint8_t)((packetlength_option & 0x03) << 5);
  SPI_write8(spi_device_id, (uint8_t)(CC112X_PKT_CFG0 & 0x7F), &val);

  val = (uint8_t)(fixedpacket_length & 0xFF);
  SPI_write8(spi_device_id, (uint8_t)(CC112X_PKT_LEN & 0x7F), &val);
}


void cc112x_cfg_rx_filterwidth(uint8_t spi_device_id, cc112x_rx_filterwidth_t filterwidth_option)
{
  uint8_t val;

  /* Assume decimation factor of 32 */
  /* RxBW = Fosc / (Decimation Factor * BB_CIC_DECFACT * 8)
     BB_CIC_DECFACT=15, RxBW = 10KHz
     BB_CIC_DECFACT=12, RxBW = 12.5KHz
     BB_CIC_DECFACT=10, RxBW = 15KHz
     BB_CIC_DECFACT=8,  RxBW = 18.75KHz
     BB_CIC_DECFACT=6,  RxBW = 25KHz
  */

  /* Note: Decimation factor of 32 selected */
  val = (uint8_t)((1 << 6) || (filterwidth_option & 0x3F));
  SPI_write8(spi_device_id, (uint8_t)(CC112X_CHAN_BW & 0x7F), &val);
}

void cc112x_cfg_msk_params(uint8_t spi_device_id, cc112x_msk_symbolrate_t msk_symbolrate)
{
  (void)spi_device_id;
  (void)msk_symbolrate;
  return;
}

void cc112x_cfg_fsk_params(uint8_t spi_device_id, cc112x_fsk_symbolrate_t fsk_symbolrate, cc112x_fsk_deviation_t fsk_deviation)
{
  uint8_t val;
  uint8_t symbolrate_exponent, deviation_exponent, deviation_mantissa;
  uint32_t symbolrate_mantissa;

  if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_6000)
  {
    symbolrate_mantissa = 0x47AE1;
    symbolrate_exponent = 0x06;
  }
  else if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_5000)
  {
    symbolrate_mantissa = 0x11111;
    symbolrate_exponent = 0x06;
  }
  else if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_4000)
  {
    symbolrate_mantissa = 0xB4E82;
    symbolrate_exponent = 0x05;
  }
  else if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_3000)
  {
    symbolrate_mantissa = 0x47AE1;
    symbolrate_exponent = 0x05;
  }
  else if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_2000)
  {
    symbolrate_mantissa = 0xB4E82;
    symbolrate_exponent = 0x04;
  }
  else if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_1200)
  {
    symbolrate_mantissa = 0x0624d;
    symbolrate_exponent = 0x04;
  }
  else if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_600)
  {
    symbolrate_mantissa = 0x0624d;
    symbolrate_exponent = 0x03;
  }
  else if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_300)
  {
    symbolrate_mantissa = 0x0624d;
    symbolrate_exponent = 0x02;
  }
  else if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_150)
  {
    symbolrate_mantissa = 0x0624d;
    symbolrate_exponent = 0x01;
  }
  else if(fsk_symbolrate == CC112X_FSK_SYMBOLRATE_50)
  {
    symbolrate_mantissa = 0x57619;
    symbolrate_exponent = 0x00;
  }

  if(fsk_deviation == CC112X_FSK_DEVIATION_8K)
  {
    deviation_mantissa = 182;
    deviation_exponent = 3;
  }
  else if(fsk_deviation == CC112X_FSK_DEVIATION_6K)
  {
    deviation_mantissa = 72;
    deviation_exponent = 3;
  }
  else if(fsk_deviation == CC112X_FSK_DEVIATION_4K)
  {
    deviation_mantissa = 181;
    deviation_exponent = 2;
  }
  else if(fsk_deviation == CC112X_FSK_DEVIATION_2K)
  {
    deviation_mantissa = 181;
    deviation_exponent = 1;
  }
  else if(fsk_deviation == CC112X_FSK_DEVIATION_1K)
  {
    deviation_mantissa = 218;
    deviation_exponent = 0;
  }
  else if(fsk_deviation == CC112X_FSK_DEVIATION_500)
  {
    deviation_mantissa = 109;
    deviation_exponent = 0;
  }
  else if(fsk_deviation == CC112X_FSK_DEVIATION_250)
  {
    deviation_mantissa = 55;
    deviation_exponent = 0;
  }
  else if(fsk_deviation == CC112X_FSK_DEVIATION_100)
  {
    deviation_mantissa = 22;
    deviation_exponent = 0;
  }


  // dev
  //5K: m=17 e=3
  //10K: m=17 e=4
  //20K: m=17 e=5

  /* Write Symbol rate */
  val = (uint8_t)(symbolrate_mantissa & 0xFF);
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_SYMBOL_RATE0), &val);
  val = (uint8_t)((symbolrate_mantissa >> 8) & 0xFF);
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_SYMBOL_RATE1), &val);
  val = (uint8_t)(((symbolrate_mantissa >> 16) & 0x0F) | ((symbolrate_exponent << 4) & 0xF0));
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_SYMBOL_RATE2), &val);

  /* Write Deviation */
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_DEVIATION_M), &deviation_mantissa);
  val = (uint8_t)((deviation_exponent & 0x07));
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_MODCFG_DEV_E), &val);
}

void cc112x_cfg_ook_params(uint8_t spi_device_id, cc112x_ook_symbolrate_t ook_symbolrate)
{
  (void)ook_symbolrate;
  /*
  {CC112X_SYNC_CFG0,         0x00}, // No sync word
  {CC112X_DEVIATION_M,       0xDA},
  {CC112X_MODCFG_DEV_E,      0x18}, // OOK
  {CC112X_PREAMBLE_CFG1,     0x00}, // No preamble
  */

  cc112x_cfg_syncword(spi_device_id, CC112x_SYNCWORD_LENGTH_DISABLED, CC112x_SYNCWORD_ERRORCHECK_DISABLED);

  cc112x_cfg_preamble(spi_device_id, CC112x_PREAMBLE_OPTION_0xAA, CC112x_PREAMBLE_LENGTH_0B);


  return;
}


void cc112x_set_config(uint8_t spi_device_id, const registerSetting_t *cfg, uint16_t len)
{
  char output[100];
  uint8_t marcstate;

  SPI_cmd(spi_device_id, CC112X_SRES);
	uint8_t writeByte;
	for(uint16_t i = 0; i < len; i++) {
        writeByte = cfg[i].data;
        cc112xSpiWriteReg(spi_device_id, cfg[i].addr, &writeByte);
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
void cc112x_manualCalibration(uint8_t spi_device_id)
{
  uint8_t original_fs_cal2;
  uint8_t calResults_for_vcdac_start_high[3];
  uint8_t calResults_for_vcdac_start_mid[3];
  uint8_t marcstate;
  uint8_t writeByte;
  char output[100];

  // 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
  writeByte = 0x00;
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_VCO2), &writeByte);

  // 2) Start with high VCDAC (original VCDAC_START + 2):
  cc112xSpiReadReg(spi_device_id, (uint8_t)(CC112X_FS_CAL2), &original_fs_cal2);
  writeByte = (uint8_t)(original_fs_cal2 + VCDAC_START_OFFSET);
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_CAL2), &writeByte);

  // 3) Calibrate and wait for calibration to be done
  //   (radio back in IDLE state)
  UART_puts(UART_INTERFACE, "Starting calibration\r\n");

  SPI_cmd(spi_device_id, CC112X_SCAL);

  do
  {
    cc112xSpiReadReg(spi_device_id, CC112X_MARCSTATE, &marcstate);
  }
  while (marcstate != 0x41);

  // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with
  //    high VCDAC_START value
  cc112xSpiReadReg(spi_device_id, (uint8_t)(CC112X_FS_VCO2), &calResults_for_vcdac_start_high[FS_VCO2_INDEX]);
  cc112xSpiReadReg(spi_device_id, (uint8_t)(CC112X_FS_VCO4), &calResults_for_vcdac_start_high[FS_VCO4_INDEX]);
  cc112xSpiReadReg(spi_device_id, (uint8_t)(CC112X_FS_CHP), &calResults_for_vcdac_start_high[FS_CHP_INDEX]);

  // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
  writeByte = 0x00;
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_VCO2), &writeByte);

  // 6) Continue with mid VCDAC (original VCDAC_START):
  writeByte = original_fs_cal2;
  cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_CAL2), &writeByte);

  // 7) Calibrate and wait for calibration to be done
  //   (radio back in IDLE state)

  SPI_cmd(spi_device_id, CC112X_SCAL);

  do
  {
    cc112xSpiReadReg(spi_device_id, CC112X_MARCSTATE, &marcstate);
  }
  while (marcstate != 0x41);

  // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained
  //    with mid VCDAC_START value
  cc112xSpiReadReg(spi_device_id, (uint8_t)(CC112X_FS_VCO2), &calResults_for_vcdac_start_mid[FS_VCO2_INDEX]);
  cc112xSpiReadReg(spi_device_id, (uint8_t)(CC112X_FS_VCO4), &calResults_for_vcdac_start_mid[FS_VCO4_INDEX]);
  cc112xSpiReadReg(spi_device_id, (uint8_t)(CC112X_FS_CHP), &calResults_for_vcdac_start_mid[FS_CHP_INDEX]);

  // 9) Write back highest FS_VCO2 and corresponding FS_VCO
  //    and FS_CHP result

  if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] >
      calResults_for_vcdac_start_mid[FS_VCO2_INDEX])
  {
    writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
    cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_VCO2), &writeByte);
    writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
    cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_VCO4), &writeByte);
    writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
    cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_CHP), &writeByte);
  }
  else
  {
    writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
    cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_VCO2), &writeByte);
    writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
    cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_VCO4), &writeByte);
    writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
    cc112xSpiWriteReg(spi_device_id, (uint8_t)(CC112X_FS_CHP), &writeByte);
  }
}
