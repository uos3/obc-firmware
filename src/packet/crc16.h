/**
 * @defgroup crc CRC
 *
 * @brief Packet CRC Routines
 *
 * @details 
 *
 * @ingroup packet
 *
 * @file crc16.h
 *
 * @{
 */

#ifndef __PACKET_CRC16_H__
#define __PACKET_CRC16_H__

#include "../firmware.h"

/**
 * Calculate the 16-bit CRC of the input buffer,
 * using the CRC16-CCITT standard.
 *
 * @param buffer The input buffer to calculated
 * @param length The length of the input buffer
 * @returns The 16bit CRC of the input buffer
 */
uint16_t Packet_CRC16_CCITT(uint8_t *buffer, uint32_t length);

#endif /* __PACKET_CRC16_H__ */

/**
 * @}
 */