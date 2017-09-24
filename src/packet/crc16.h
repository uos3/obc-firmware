/* @file crc16.h
 * @ingroup packet
 * @{
 */

#ifndef __PACKET_CRC16_H__
#define __PACKET_CRC16_H__

#include "../firmware.h"

uint16_t CRC16_CCITT(uint8_t *buffer, uint32_t length);

#endif /* __PACKET_CRC16_H__ */

/**
 * @}
 */