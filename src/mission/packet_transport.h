#ifndef TRANSPORT_H
#define TRANSPORT_H
/*
	Provides transport layer functions as defined in TMTC_20191127_v4.6
*/

#include "packet_base.h"
#include "buffer.h"


#define PACKET_TYPE_ACK 0xF
#define PACKET_TYPE_DAT 0x0

// can use bit field addresses to construct the transport information section
#pragma pack(1)
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
typedef struct transport_info_t
{
	uint8_t packet_type : 4;
	uint8_t start_of_sequence : 1;
	uint8_t end_of_sequence : 1;
	uint8_t initial_packet : 1;
	uint8_t do_not_continue : 1;
} transport_info_t;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
typedef struct transport_info_t
{
	uint8_t do_not_continue : 1;
	uint8_t initial_packet : 1;
	uint8_t end_of_sequence : 1;
	uint8_t start_of_sequence : 1;
	uint8_t packet_type : 4;
} transport_info_t;
#endif


#pragma pack(1)
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
typedef struct transport_header_struct{
	PACKET_SEQUENCE_TYPE sequence_number;
	transport_info_t info;
}transport_header_struct;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
typedef struct transport_header_struct{
	PACKET_SEQUENCE_TYPE sequence_number;
	transport_info_t info;
}transport_header_struct;
#endif

typedef union transport_header_t{
	transport_header_struct as_struct;
	uint8_t as_bytes[sizeof(transport_header_struct)];
}transport_header_t;


transport_header_t transport_header_fromfields(uint16_t seq_num, uint8_t type, uint8_t is_start, uint8_t is_end, uint8_t is_init, uint8_t is_do_not_continue);

uint32_t packet_write_transport_to_buffer(uint16_t buffer_block_number, transport_info_t transport_info);

uint16_t packet_prep_transport();

#endif