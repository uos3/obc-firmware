
#ifndef PACKET_H
#define PACKET_H

#include "packet_base.h"
#include "packet_auth.h"
#include "packet_transport.h"
#include "packet_application.h"

/*
	should mirror each blocks structure in the buffer.
	As much as I'd like to do clever things with some weird type where you can
	have arbitary start points for non-standard length members, this is easier
*/
#pragma pack(1)

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
typedef struct packet_typed_struct{
	uint8_t data_bytes[BLOCK_SIZE-BUFFER_DATA_START_INDEX];
	transport_header_struct transport_header;
	packet_hash_t hash;
	uint8_t length;
}packet_typed_struct;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
typedef struct packet_typed_struct{
	uint8_t length;
	packet_hash_t hash;
	transport_header_struct transport_header;
	uint8_t data_bytes[BLOCK_SIZE-BUFFER_DATA_START_INDEX];
}packet_typed_struct;
#endif

typedef union packet_typed_t{
	packet_typed_struct as_struct;
	uint8_t as_bytes[sizeof(packet_typed_struct)];
} packet_typed_t;

void store_payload_data(uint8_t whofor, uint8_t* data, uint32_t data_len);

uint16_t packet_prep_transport();

#endif