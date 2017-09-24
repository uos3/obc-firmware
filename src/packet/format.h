/* @file format.h
 * @addtogroup packet
 * @{
 */

#ifndef __PACKET_FORMAT_H__
#define __PACKET_FORMAT_H__

#include "../firmware.h"

#define PACKET_SPACECRAFT_ID 0x4242;

typedef struct coded_data { //1048 bits
  uint16_t spacecraft;
  uint16_t type;
  uint16_t _unused;
  uint8_t data[1024];
  uint8_t hash[16];
  uint16_t crc;
} packet_coded_data;

#endif /* __PACKET_FORMAT_H__ */

/**
 * @}
 */