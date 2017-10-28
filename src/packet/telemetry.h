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

typedef struct packet_telemetry_realtime {
  uint8_t obc_temperature;
  uint8_t receiver_temperature;
  uint8_t transmitter_temperature;
  uint8_t pa_temperature;
  
} packet_telemetry_realtime;


#endif /* __PACKET_TELEMETRY_H__ */