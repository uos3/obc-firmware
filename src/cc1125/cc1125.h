#ifndef __CC112x_H__
#define __CC112x_H__

#include "../firmware.h"

typedef struct
{
  uint16_t  addr;
  uint8_t   data;
}registerSetting_t;

bool cc1125_pollGPIO(uint8_t gpio_pin);
void manualCalibration(uint8_t radio_id);
void radio_reset_config(uint8_t radio_id, registerSetting_t *cfg, uint16_t len);
uint8_t radio_set_pwr_f(uint8_t radio_id, double *pwr, uint8_t *reg_value);
uint8_t radio_set_freq_f(uint8_t radio_id, double *freq);

#endif /* __CC112x_H__ */