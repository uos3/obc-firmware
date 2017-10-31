/**
 * @defgroup packet
 *
 * @brief Telemetry Definitions
 *
 * @details 
 *
 * @ingroup packet
 */

#ifndef __PACKET_TELEMETRY_H__
#define __PACKET_TELEMETRY_H__

#include "../firmware.h"

Time  32
time_source 2
sequence_id 16
obc_temperature 8
battery_temperature 8
battery_voltage 8
battery_current 8
charge_current  8
antenna_deployment  1
data_pending  8
reboot_count  8
rails_status  6
rx_temperature  8
tx_temperature  8
pa_temperature  8
rx_noisefloor 8


typedef struct packet_telemetry_realtime {
  uint32_t time;
  uint16_t sequence;
  uint8_t battery_voltage;
  uint8_t battery_current;
  uint8_t battery_temperature;
  uint8_t obc_temperature;
  uint8_t rx_temperature;
  uint8_t tx_temperature;
  uint8_t pa_temperature;

} packet_telemetry_realtime;


#endif /* __PACKET_TELEMETRY_H__ */