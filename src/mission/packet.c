
#include "packet_base.h"
#include "packet_auth.h"
#include "packet_transport.h"
#include "packet_application.h"
#include "packet.h"
#include "buffer.h"
#include "../utility/debug.h"


void packet_prep_buffer(){
	uint8_t current_block_length;
	uint8_t block_remaining_length;

	// the total amount of bytes in the block, including reserved
	current_block_length = buffer_status.as_struct.current_block_position;
	// the total writable length. Remember, first byte in block is the length/status byte.
	block_remaining_length = BLOCK_SIZE - current_block_length;

	/*
		if we can't fit the header in the packet, there's no point writing.
		we then move to the next block. No partial headers, please.
	*/
	if (block_remaining_length <= APPLICATION_HEADER_LEN){
		buffer_increment_block();
	}
}


void store_payload_data(uint8_t whofor, uint8_t* data, uint32_t data_len){
	app_header_t header;
	uint8_t is_unfinished;

	// first, prep the buffer to prevent incomplete headers
	packet_prep_buffer();

	// build the header
	header.as_struct.length = data_len;
	is_unfinished = app_is_unfinished(data_len);
	header.as_struct.info = app_info_fromfields(whofor, is_unfinished, 0, 0, 0);

	// write to buffer
	buffer_write_next(header.as_bytes, APPLICATION_HEADER_LEN);
	buffer_write_next(data, data_len);
}

uint16_t packet_prep_transport(){
	// retreive status from buffer
	uint16_t first_block_num, last_block_num, seq_num, block_num;
	transport_header_t current_header;
	first_block_num = buffer_status.as_struct.transmit_block_address;
	last_block_num = buffer_status.as_struct.current_block_address;

	// little bit of logic to asset that the for loop will run
	if (last_block_num < first_block_num){
		// number of blocks -1 is the number of the last block
		last_block_num += (BUFFER_BLOCKS - 1 - first_block_num);
		debug_printf("packet_transport.c: warning: changed block numbering");
	}
	seq_num = first_block_num;
	// first block, start of sequence raised:
	current_header = transport_header_fromfields(seq_num, PACKET_TYPE_DAT, 1, 0, 0, 0);
	packet_write_transport_to_buffer(seq_num, current_header);
	seq_num++;
	for(seq_num ; seq_num < last_block_num; seq_num++){
		block_num = seq_num % BUFFER_BLOCKS;
		// debug_printf("prepping transport for block %d", seq_num);
		// make transport header
		current_header = transport_header_fromfields(seq_num, PACKET_TYPE_DAT, 0, 0, 0, 0);
		packet_write_transport_to_buffer(seq_num, current_header);
	}
	// for loop exits when seq_num = last_block_num
	current_header = transport_header_fromfields(seq_num, PACKET_TYPE_DAT, 0, 1, 0, 0);
	packet_write_transport_to_buffer(seq_num, current_header);
	return seq_num++;
}

uint8_t* packet_process_sequence(uint32_t start_block_num){
	// check that the packet is actually start of sequence
	
}
