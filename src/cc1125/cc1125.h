
#include "../board/hal_spi_rf_trxeb.h"
#include "inttypes.h"

void manualCalibration(uint8_t radio_id);
void radio_reset_config(uint8_t radio_id, registerSetting_t *cfg, uint16_t len);