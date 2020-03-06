#include "packet/packet_base.h"
#include "packet/packet_auth.h"
#include "packet/packet_transport.h"
#include "packet/packet_application.h"

/*
	should mirror each blocks structure in the buffer.
	As much as I'd like to do clever things with some weird type where you can
	have arbitary start points for non-standard length members, this is easier
*/
typedef struct typed_packet_t{
	uint8_t length;
	packet_hash_t hash;
	transport_header_t transport_header;
	uint8_t data_bytes[BLOCK_SIZE-BUFFER_DATA_START_INDEX];
}typed_packet_t;
