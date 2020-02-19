/*
	Packet authentication layer
	Author: Richard A
	Last modified: 2020 02 19
	Status: Functional
*/

#include <stdlib.h>
#include <string.h>

#include "packet_base.h"
// #include "driverlib/shamd5.h" // my hopes and dreams shattered by model number
#include "../utility/shake.h"
#include "../utility/debug.h"

#ifdef DEBUG_MODE
	char output[256];
	#include <stdio.h>
#endif


// TEMPORARY
static const uint8_t PACKET_SECRET_WORD[] = "RichardWasHere";


void packet_hasher(Packet* packet){
	uint32_t data_length = packet_get_data_len(packet);
	uint32_t combined_length = data_length + sizeof(PACKET_SECRET_WORD);
	// create a data buffer that contains both the packet and the secret word
	uint8_t *combined_data = malloc(combined_length*sizeof(uint8_t));

	if (combined_data == NULL){
		return;
	}

	// put the packet data in
	memcpy(combined_data, packet->data,  data_length);
	// put the secret after the data
	memcpy(&combined_data[data_length], PACKET_SECRET_WORD, sizeof(PACKET_SECRET_WORD));
	#ifdef DEBUG_MODE
		debug_print("PACKET_AUTH.C: combined data");
		debug_hex(combined_data, combined_length);
	#endif
	
	util_shake_ctx_t shake_it_up;
	Util_shake_init(&shake_it_up, PACKET_HASH_LEN);
	Util_shake_update(&shake_it_up, combined_data, combined_length);
	Util_shake_out(&shake_it_up, packet->hash);
	// no longer need combined data, so free.
	free(combined_data);
}


uint8_t packet_is_authentic(Packet* packet){
	uint8_t packet_hash_buffer[PACKET_HASH_LEN];
	memcpy(packet_hash_buffer, packet->hash, PACKET_HASH_LEN);

	#ifdef DEBUG_MODE
		debug_print("PACKET_AUTH.C original hash:");
		debug_hex(packet->hash, PACKET_HASH_LEN);
	#endif

	// copy hash to temp

	// recalculate hash based on packet data content
	packet_hasher(packet);
	#ifdef DEBUG_MODE
		debug_print("PACKET_AUTH.C new hash:");
		debug_hex(packet->hash, PACKET_HASH_LEN);
	#endif

	if (memcmp(packet_hash_buffer, packet->hash, PACKET_HASH_LEN) == 0){
		// true, no differences in the hashes
		return 0xFF;
	}
	// false
	return 0x00;
}