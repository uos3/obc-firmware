#include <stdint.h>

#include "packet_base.h"

Packet packet_constructor(){
	Packet packet;
	packet.length = 0;
	packet.auth_start = &packet.packet_buffer;
	packet.transport_start = &packet.auth_start + PACKET_HASH_LEN;
	packet.application_start = &packet.transport_start + TRANSPORT_LEN;
	return packet;
}

uint32_t packet_get_data_len(Packet* packet){
	return packet->length - PACKET_HASH_LEN;
}

