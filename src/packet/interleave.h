/* @file interleave.h
 * @ingroup packet
 * @{
 */

#ifndef __PACKET_INTERLEAVE_H__
#define __PACKET_INTERLEAVE_H__

#include "../firmware.h"

void packet_interleave_32x32(uint8_t *buffer);

#define	packet_uninterleave_32x32(b)		packet_interleave_32x32(b)

#endif /* __PACKET_INTERLEAVE_H__ */

/**
 * @}
 */