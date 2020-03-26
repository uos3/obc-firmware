/*
	Packet authentication layer
	Author: Richard A
	Last modified: 2020 03 13
	Status: Functional
*/

#include <stdlib.h>
#include <string.h>

#include "packet_base.h"
#include "packet_auth.h"
// #include "driverlib/shamd5.h" // my hopes and dreams shattered by model number
#include "../utility/shake.h"
#include "../utility/debug.h"
#include "../utility/byte_plexing.h"

#ifdef DEBUG_MODE
	char output[256];
	#include <stdio.h>
#endif


// TEMPORARY
static const uint8_t PACKET_SECRET_WORD[] = "RichardWasHere";


void auth_hash_data(auth_struct* output_hash, uint8_t* data, uint8_t data_length){
	uint32_t combined_length = data_length + sizeof(PACKET_SECRET_WORD);
	// create a data buffer that contains both the packet and the secret word
	uint8_t *combined_data = malloc(combined_length*sizeof(uint8_t));

	if (combined_data == NULL){
		return;
	}

	// put the packet data in
	memcpy(combined_data, data,  data_length);
	// put the secret after the data
	memcpy(&combined_data[data_length], PACKET_SECRET_WORD, sizeof(PACKET_SECRET_WORD));

	// #ifdef DEBUG_MODE
	// 	debug_print("PACKET_AUTH.C: combined data");
	// 	debug_hex(combined_data, combined_length);
	// #endif
	
	util_shake_ctx_t shake_it_up;
	Util_shake_init(&shake_it_up, AUTH_LEN);
	Util_shake_update(&shake_it_up, combined_data, combined_length);
	Util_shake_out(&shake_it_up, cast_asptr(output_hash->as_bytes));
	// no longer need combined data, so free.
	free(combined_data);
}


bool auth_matches_data(auth_struct* recieved_hash, uint8_t* data, uint8_t data_length){
	auth_struct calculated_hash;

	// calculate hash
	auth_hash_data(&calculated_hash, data, data_length);

	// #ifdef DEBUG_MODE
	// 	debug_print("PACKET_AUTH.C: new hash:");
	// 	debug_hex(calculated_hash.as_bytes, AUTH_LEN);
	// #endif

	if (memcmp(cast_asptr(recieved_hash->as_bytes), cast_asptr(calculated_hash.as_bytes), AUTH_LEN) == 0){
		// true, no differences in the hashes
		return true;
	}
	return false;
}