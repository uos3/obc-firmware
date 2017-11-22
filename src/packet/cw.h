/**
 * @defgroup cw CW
 *
 * @brief CW (Morse Code) Routines
 *
 * @details 
 *
 * @ingroup packet
 */
#ifndef __PACKET_CW_H__
#define __PACKET_CW_H__

#include "../firmware.h"

void Packet_cw_transmit_buffer(uint8_t *cw_buffer, uint32_t cw_length, void *_cw_on(void), void *_cw_off(void));

#endif /* __PACKET_CW_H__ */