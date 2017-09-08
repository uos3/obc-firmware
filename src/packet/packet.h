#ifndef __PACKET_H__
#define __PACKET_H__

#include "../firmware.h"

#define PACKET_SPACECRAFT_ID 0x4242;

typedef struct coded_data { //1048 bits
  uint16_t spacecraft;
  uint16_t type;
  uint16_t _unused;
  uint8_t data[1024];
  uint8_t hash[16];
  uint16_t crc;
} coded_data;

void packet_scramble_xor(uint8_t *buffer, uint32_t length);

void packet_interleave(uint8_t *in, uint8_t *out, uint32_t x, uint32_t y);

#endif /* __PACKET_H__ */