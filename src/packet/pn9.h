/**
 * @defgroup pn9 PN9
 *
 * @brief Packet PN9 Routines
 *
 * @details 
 *
 * @ingroup packet
 */
#ifndef __PACKET_PN9_H__
#define __PACKET_PN9_H__

#include "../firmware.h"

void Packet_pn9_xor(uint8_t *buffer, uint32_t length);

#endif /* __PACKET_PN9_H__ */