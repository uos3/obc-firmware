#ifndef __CRC16_H__
#define __CRC16_H__

#include "../firmware.h"

uint16_t CRC16_CCITT(uint8_t *buffer, uint32_t length);

#endif /* __CRC16_H__ */