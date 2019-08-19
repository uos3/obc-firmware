#ifndef __CC1125_H__
#define __CC1125_H__

//#include "../firmware.h"
#include "../cc112x/cc112x.h"
#include "../cc112x/cc112x_spi.h"
// static typedef struct
// {
//   uint16_t  addr;
//   uint8_t   data;
// }registerSetting_t;

bool cc1125_pollGPIO(uint8_t gpio_pin);
void manualCalibration(uint8_t radio_id);
void radio_reset_config(uint8_t radio_id, const registerSetting_t *cfg, uint16_t len);
uint8_t radio_set_pwr_f(uint8_t radio_id, double *pwr, uint8_t *reg_value);
uint8_t radio_set_freq_f(uint8_t radio_id, double *freq);
uint8_t radio_set_fsk_param(uint8_t radio_id, uint32_t *symbol_rate, uint32_t *deviation);
uint8_t radio_set_rxbw_param(uint8_t radio_id, uint32_t *rxbw);
uint8_t radio_set_pwr_reg(uint8_t radio_id, uint8_t reg_value);
double radio_pwr_reg_to_dbm(uint8_t reg_value);

#endif /* __CC1125_H__ */