/* @file interleave.h
 *
 * @ingroup packet
 *
 * @{
 */

#ifndef __PACKET_INTERLEAVE_H__
#define __PACKET_INTERLEAVE_H__

#include "../firmware.h"

void Packet_interleave_32x32(uint8_t *buffer);
void Packet_interleave_64x64(uint8_t *buffer);

#define	Packet_uninterleave_32x32(b)		Packet_interleave_32x32(b)
#define	Packet_uninterleave_64x64(b)		Packet_interleave_64x64(b)

#endif /* __PACKET_INTERLEAVE_H__ */

/**
 * @}
 */