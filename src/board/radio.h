/**
 * @defgroup radio
 *
 * @brief Radio Peripheral Driver
 *
 * @details Hardware Radio Peripheral Driver.
 *
 * @ingroup drivers
 */

#ifndef __RADIO_H__
#define __RADIO_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct radio_config_t{
	double frequency; // MHz
	double power; // dBm
}radio_config_t ;

typedef enum {
	RADIO_STATUS_OK,
	RADIO_STATUS_BUSY,
	RADIO_STATUS_WRONGPART
} Radio_Status_t;

Radio_Status_t Radio_tx_msk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler(void));

Radio_Status_t Radio_tx_fsk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler(void));

Radio_Status_t Radio_tx_morse(radio_config_t *radio_config, uint8_t *text_buffer, uint32_t text_length, void *end_of_tx_handler(void));

Radio_Status_t Radio_tx_off(radio_config_t *radio_config);

Radio_Status_t Radio_rx_receive(radio_config_t *radio_config, uint8_t *receive_buffer, uint32_t receive_length, void received_packet_handler(void));

Radio_Status_t Radio_rx_off(radio_config_t *radio_config);


#endif /*  __RADIO_H__ */
/*
static const registerSetting_t preferredSettings_fsk[]= 
{
  {CC112X_IOCFG3,            0x59},
  {CC112X_IOCFG2,            0x13},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            0x06},
  {CC112X_SYNC_CFG1,         0x0B},
  {CC112X_DEVIATION_M,       0xDA},
  {CC112X_MODCFG_DEV_E,      0x00},
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_FREQ_IF_CFG,       0x40},
  {CC112X_IQIC,              0x46},
  {CC112X_CHAN_BW,           0x69},
  {CC112X_MDMCFG0,           0x05},
  {CC112X_SYMBOL_RATE2,      0x3B},
  {CC112X_SYMBOL_RATE1,      0x4E},
  {CC112X_SYMBOL_RATE0,      0x82},
  {CC112X_AGC_REF,           0x15},
  {CC112X_AGC_CS_THR,        0x19},
  {CC112X_AGC_CFG1,          0xA0},
  {CC112X_AGC_CFG0,          0xCF},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x1B},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_PKT_CFG1,          0x05},
  {CC112X_PA_CFG0,           0x7E},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x22},
  {CC112X_FREQ2,             0x5A},
  {CC112X_FREQ1,             0x9F},
  {CC112X_FREQ0,             0xFF},
  {CC112X_IF_ADC0,           0x05},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC3,             0xC7},
  {CC112X_XOSC1,             0x07},
};

static const registerSetting_t preferredSettings_cw[]= 
{
   {CC112X_IOCFG3,            0x59},
   {CC112X_IOCFG2,            0x13},
   {CC112X_IOCFG1,            0xB0},
   {CC112X_IOCFG0,            0x06},
   {CC112X_SYNC_CFG1,         0x08},
   {CC112X_DEVIATION_M,       0xB4},
   {CC112X_MODCFG_DEV_E,      0x0A},
   {CC112X_DCFILT_CFG,        0x1C},
   {CC112X_PREAMBLE_CFG1,     0x00},
   {CC112X_FREQ_IF_CFG,       0x35},
   {CC112X_IQIC,              0xC6},
   {CC112X_CHAN_BW,           0x10},
   {CC112X_MDMCFG1,           0x06},
   {CC112X_MDMCFG0,           0x05},
   {CC112X_SYMBOL_RATE2,      0x40},
   {CC112X_SYMBOL_RATE1,      0x62},
   {CC112X_SYMBOL_RATE0,      0x4E},
   {CC112X_AGC_REF,           0x20},
   {CC112X_AGC_CS_THR,        0x19},
   {CC112X_AGC_CFG1,          0xA9},
   {CC112X_AGC_CFG0,          0xCF},
   {CC112X_FIFO_CFG,          0x00},
   {CC112X_SETTLING_CFG,      0x03},
   {CC112X_FS_CFG,            0x1B},
   {CC112X_PKT_CFG2,          0x05},
   {CC112X_PKT_CFG1,          0x00},
   {CC112X_PKT_CFG0,          0x20},
   {CC112X_PA_CFG2,           0x34},
   {CC112X_PA_CFG0,           0x7E},
   {CC112X_IF_MIX_CFG,        0x00},
   {CC112X_FREQOFF_CFG,       0x22},
   {CC112X_CFM_DATA_CFG,      0x01},
   {CC112X_FREQ2,             0x5A},
   {CC112X_FREQ1,             0xEF},
   {CC112X_FREQ0,             0xFF},
   {CC112X_IF_ADC0,           0x05},
   {CC112X_FS_DIG1,           0x00},
   {CC112X_FS_DIG0,           0x5F},
   {CC112X_FS_CAL0,           0x0E},
   {CC112X_FS_DIVTWO,         0x03},
   {CC112X_FS_DSM0,           0x33},
   {CC112X_FS_DVC0,           0x17},
   {CC112X_FS_PFD,            0x50},
   {CC112X_FS_PRE,            0x6E},
   {CC112X_FS_REG_DIV_CML,    0x14},
   {CC112X_FS_SPARE,          0xAC},
   {CC112X_XOSC5,             0x0E},
   {CC112X_XOSC3,             0xC7},
   {CC112X_XOSC1,             0x07},
   {CC112X_SERIAL_STATUS,     0x08},
};
*/
