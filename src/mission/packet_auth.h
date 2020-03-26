#ifndef PACKET_AUTH
#define PACKET_AUTH

#include <stdint.h>
#include <stdbool.h>
#include "packet_base.h"

typedef struct auth_struct{
	uint8_t as_bytes[16];
}auth_struct;

void auth_hash_data(auth_struct* output_hash, uint8_t* data, uint8_t data_length);

bool auth_matches_data(auth_struct* recieved_hash, uint8_t* data, uint8_t data_length);

#endif /* packet auth header */