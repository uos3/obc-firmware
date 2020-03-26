#include "../utility/debug.h"
#include "../mission/packet_transport.h"
#include "../mission/buffer.h"
#include <stdio.h>

char output[256];

int main(){
	debug_init();
	debug_clear();
	debug_print("=== demo transport layer  ===");
	transport_info_t trans_inf;
	uint16_t sequence_number;
	uint8_t trans_inf_byte;
	// uint8_t is_start, is_end, is_init, is_do_not_continue

	sequence_number = 0x0A01;
	trans_inf = transport_info_fromfields(0x00, 0xFF, 0xFF, 0x00, 0x00);
	
	debug_print("getting transport info as byte, should be 0x0C");
	trans_inf_byte = packet_transport_info_asbyte(trans_inf);
	output[0] = trans_inf_byte;
	debug_hex(output, 1);

	debug_print("writing some arbitary data to buffer to appease the check");
	_buffer_overwrite_table();
	buffer_init();
	buffer_write_next("hello world!", 13);

	debug_print("writing sequence number to buffer");
	sprintf(output, "packet_sequence start index %u dsi: %u", PACKET_SEQUENCE_START_INDEX, BUFFER_DATA_START_INDEX);
	debug_print(output);

	packet_write_sequence_to_buffer(1, sequence_number);

	debug_print("writing transport info to buffer");
	packet_write_transport_to_buffer(1, trans_inf);

	debug_print("retrieving 'next transmit' from buffer");
	uint8_t length_in_block = 0;
	buffer_retrieve_next_transmit(output, &length_in_block);
	debug_print("sequence number: ");
	debug_hex((uint8_t *) &output+PACKET_SEQUENCE_START_INDEX, PACKET_SEQUENCE_LEN);
	debug_print("transport info: ");
	debug_hex((uint8_t *) &output+TRANSPORT_INFO_START_INDEX, TRANSPORT_INFO_LEN);
	debug_print("data: ");
	debug_print((uint8_t *) &output+BUFFER_DATA_START_INDEX);

	// debug_print("block dump");
	// debug_hex((uint8_t *) &output+PACKET_SEQUENCE_START_INDEX-1, length_in_block-PACKET_SEQUENCE_START_INDEX+1);

	debug_print("=== end demo ===");
	debug_end();
	return 0;
}