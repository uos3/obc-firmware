#ifndef PACKET_AUTH
#define PACKET_AUTH

#include <stdint.h>
#include "packet_base.h"

typedef struct packet_hash_t{
	uint8_t bytes[16];
}packet_hash_t;

void packet_hasher(Packet* packet);

uint8_t packet_is_authentic(Packet* packet);

#endif /* packet auth header */