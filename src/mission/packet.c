#include "packet_base.h"
#include "packet_auth.h"
#include "packet_transport.h"
#include "packet_application.h"
#include "buffer.h"


void packet_prep_buffer(){
	uint8_t current_block_length;
	uint8_t block_remaining_length;

	// the total amount of bytes in the block, including reserved
	current_block_length = buffer_status.current_block_position;
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


uint64_t packet_is_unfinished(data_len){
	if (data_len > buffer_status.current_block_position){
		return 0xFF;
	}
	return 0x00;
}


void store_payload_data(uint8_t whofor, uint8_t data, uint32_t data_len){
	app_header_t header;
	uint8_t is_unfinished;

	// first, prep the buffer to prevent incomplete headers
	packet_prep_buffer();

	// build the header
	header.as_struct.length = data_len;
	is_unfinished = packet_is_unfinished(data_len);
	header.as_struct.info = app_info_fromfields(whofor, is_unfinished, 0, 0, 0);

	// write to buffer
	buffer_write_next(header.as_bytes, APPLICATION_HEADER_LEN);
	buffer_write_next(data, data_len);
}
