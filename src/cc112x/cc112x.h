#ifndef __CC112x_H__
#define __CC112x_H__

#include "../firmware.h"

#define CC_XO_FREQ 38400000

/*** Device Control ***/

void cc112x_reset(uint8_t spi_device_id);

void cc112x_powerdown(uint8_t spi_device_id);

uint8_t cc112x_query_partnumber(uint8_t spi_device_id);

void cc112x_manualCalibration(uint8_t spi_device_id);

/*** Configuration ***/

typedef struct registerSetting_t{
  uint16_t  addr;
  uint8_t   data;
} registerSetting_t;

typedef enum {
  CC112x_BANDSEL_OPTION_136M = 11,
  CC112x_BANDSEL_OPTION_410M = 4,
} cc112x_bandsel_option_t;

void cc112x_cfg_frequency(uint8_t spi_device_id, cc112x_bandsel_option_t bandsel_option, uint32_t _frequency_word);

void cc112x_cfg_power(uint8_t spi_device_id, int8_t power_dbm);

typedef enum {
  CC112x_PREAMBLE_OPTION_0xAA = 0,
  CC112x_PREAMBLE_OPTION_0x55 = 1,
  CC112x_PREAMBLE_OPTION_0x33 = 2,
  CC112x_PREAMBLE_OPTION_0xCC = 3,
} cc112x_preamble_option_t;

typedef enum {
  CC112x_PREAMBLE_LENGTH_0B = 0,
  CC112x_PREAMBLE_LENGTH_1B = 2,
  CC112x_PREAMBLE_LENGTH_2B = 4,
  CC112x_PREAMBLE_LENGTH_3B = 5,
  CC112x_PREAMBLE_LENGTH_4B = 6,
  CC112x_PREAMBLE_LENGTH_6B = 8,
  CC112x_PREAMBLE_LENGTH_12B = 11,
  CC112x_PREAMBLE_LENGTH_24B = 12,
} cc112x_preamble_length_t;

void cc112x_cfg_preamble(uint8_t spi_device_id, cc112x_preamble_option_t preamble_option, cc112x_preamble_length_t preamble_length);

typedef enum {
  CC112x_SYNCWORD_LENGTH_DISABLED = 0,
  CC112x_SYNCWORD_LENGTH_11b = 1,
  CC112x_SYNCWORD_LENGTH_16b = 2,
  CC112x_SYNCWORD_LENGTH_18b = 3,
  CC112x_SYNCWORD_LENGTH_24b = 4,
  CC112x_SYNCWORD_LENGTH_32b = 5,
} cc112x_syncword_length_t;

typedef enum {
  CC112x_SYNCWORD_ERRORCHECK_0b = 0,
  CC112x_SYNCWORD_ERRORCHECK_lt2b = 1,
  CC112x_SYNCWORD_ERRORCHECK_DISABLED = 2,
} cc112x_syncword_errorcheck_t;

void cc112x_cfg_syncword(uint8_t spi_device_id, cc112x_syncword_length_t syncword_length, cc112x_syncword_errorcheck_t syncword_errorcheck);

typedef enum {
  CC112x_PACKETLENGTH_FIXED = 0,
  CC112x_PACKETLENGTH_VARIABLE = 1,
} cc112x_packetlength_t;

void cc112x_cfg_packetlength(uint8_t spi_device_id, cc112x_packetlength_t packetlength_option, uint8_t fixedpacket_length);

typedef enum {
  CC112X_RX_FILTERWIDTH_10K = 15,
  CC112X_RX_FILTERWIDTH_12K5 = 12,
  CC112X_RX_FILTERWIDTH_15K = 10,
  CC112X_RX_FILTERWIDTH_18K75 = 8,
  CC112X_RX_FILTERWIDTH_25K = 6,
} cc112x_rx_filterwidth_t;

void cc112x_cfg_rx_filterwidth(uint8_t spi_device_id, cc112x_rx_filterwidth_t filterwidth_option);

typedef enum {
  CC112X_MSK_SYMBOLRATE_100, // 0x00, 0xaec33 (99)
  CC112X_MSK_SYMBOLRATE_250, // 0x01, 0xb4e81 (249)
  CC112X_MSK_SYMBOLRATE_500, // 0x02, 0xb4e81 (499)
  CC112X_MSK_SYMBOLRATE_1K,  // 0x03, 0xb4e81 (999)
  CC112X_MSK_SYMBOLRATE_2K,  // 0x04, 0xb4e81 (1999)
  CC112X_MSK_SYMBOLRATE_4K,  // 0x05, 0xb4e81 (3999)
  CC112X_MSK_SYMBOLRATE_6K,  // 0x06, 0x47ae1 (5999)
} cc112x_msk_symbolrate_t;

void cc112x_cfg_msk_params(uint8_t spi_device_id, cc112x_msk_symbolrate_t msk_symbolrate);

typedef enum {
  CC112X_FSK_SYMBOLRATE_50,  // 0x00, 0x57619 (49)
  CC112X_FSK_SYMBOLRATE_150, // 0x01, 0x0624d (149)
  CC112X_FSK_SYMBOLRATE_300, // 0x02, 0x0624d (299)
  CC112X_FSK_SYMBOLRATE_600, // 0x03, 0x0624d (599)
  CC112X_FSK_SYMBOLRATE_1200, // 0x04, 0x0624d (1199)
} cc112x_fsk_symbolrate_t;

typedef enum {
  CC112X_FSK_DEVIATION_100,
  CC112X_FSK_DEVIATION_250,
  CC112X_FSK_DEVIATION_500,
  CC112X_FSK_DEVIATION_1K,
  CC112X_FSK_DEVIATION_2K,
  CC112X_FSK_DEVIATION_4K,
  CC112X_FSK_DEVIATION_6K,
  CC112X_FSK_DEVIATION_8K,
} cc112x_fsk_deviation_t;

void cc112x_cfg_fsk_params(uint8_t spi_device_id, cc112x_fsk_symbolrate_t fsk_symbolrate, cc112x_fsk_deviation_t fsk_deviation);

typedef enum {
  CC112X_OOK_SYMBOLRATE_0_1,
} cc112x_ook_symbolrate_t;

void cc112x_cfg_ook_params(uint8_t spi_device_id, cc112x_ook_symbolrate_t ook_symbolrate);

void cc112x_set_config(uint8_t spi_device_id, const registerSetting_t *cfg, uint16_t len);

#endif /* __CC112x_H__ */