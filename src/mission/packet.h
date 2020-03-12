
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
	auth_struct hash;
	uint8_t length;
}packet_typed_struct;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
typedef struct packet_typed_struct{
	uint8_t length;
	auth_struct hash;
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

void packet_retrieve_from_buffer(uint16_t block_num, packet_typed_t* packet_ptr, uint8_t* packet_length_ptr);

uint32_t packet_get_app_data_from_buffer(uint16_t block_num, uint8_t app_header_start_index, uint8_t* app_buffer, app_header_t app_header);

app_header_t packet_get_app_header_from_buffer(uint16_t block_num, uint8_t app_header_start_index);



#endif