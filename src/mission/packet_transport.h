#ifndef TRANSPORT_H
#define TRANSPORT_H
/*
	Provides transport layer functions as defined in TMTC_20191127_v4.6
*/

#include "packet_base.h"
#include "buffer.h"

#define PACKET_TYPE_ACK 0xF
// can use bit field addresses to construct the transport information section
typedef struct transport_info_t
{
	int packet_type : 4;
	int start_of_sequence : 1;
	int end_of_sequence : 1;
	int initial_packet : 1;
	int do_not_continue : 1;
} transport_info_t;


typedef struct transport_header_t{
	PACKET_SEQUENCE_TYPE sequence_number;
	transport_info_t info;
}transport_header_t;


uint8_t transport_info_asbyte(transport_info_t transport_info);

transport_info_t transport_info_frombyte(uint8_t byte);

transport_info_t transport_info_fromfields(uint8_t packet_type, uint8_t is_start, uint8_t is_end, uint8_t is_init, uint8_t is_do_not_continue);

uint32_t packet_write_sequence_to_buffer(uint16_t buffer_block_number, uint16_t sequence_number);

uint32_t packet_write_transport_to_buffer(uint16_t buffer_block_number, transport_info_t transport_info);

#endif