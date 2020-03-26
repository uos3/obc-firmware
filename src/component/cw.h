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

// #include "../board/radio.h"
// #include <stddef.h>
#include <inttypes.h>
#include "radio.h"
// #include <stdbool.h>

void Packet_cw_transmit_buffer(uint8_t *cw_buffer, uint32_t cw_length, radio_config_t *Radio_config, void (*_cw_on)(radio_config_t *), void (*_cw_off)(void));// void _cw_on(radio_config_t *), void _cw_off(void));

#endif /* __PACKET_CW_H__ */