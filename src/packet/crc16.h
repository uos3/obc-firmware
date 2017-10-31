/**
 * @defgroup crc
 *
 * @brief Packet CRC Routines
 *
 * @details 
 *
 * @ingroup packet
 */

#ifndef __PACKET_CRC16_H__
#define __PACKET_CRC16_H__

#include "../firmware.h"

/**
 * @function
 * @ingroup crc
 *
 * Calculate the 16-bit CRC of the input buffer,
 * using the CRC16-CCITT standard.
 *
 * @param input Input buffer to calculated
 * @param length Length of the input buffer
 * @param output Pointer to store the output
 * @returns The 16bit CRC of the input buffer
 */
void Packet_crc16(uint8_t *input, uint32_t length, uint16_t *output);

#endif /* __PACKET_CRC16_H__ */