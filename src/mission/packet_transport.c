/*
	Provides transport layer functions as defined in TMTC_20191127_v4.6
*/

#include "packet_base.h"
#include "buffer.h"
#include "packet_transport.h"
#include "../utility/debug.h"
#include "../utility/byte_plexing.h"
// #include <stdio.h>

/* Writing packets */
transport_info_t transport_info_fromfields(uint8_t type, uint8_t is_start, uint8_t is_end, uint8_t is_init, uint8_t is_do_not_continue){
	transport_info_t info;
	// info.packet_type = (type & 0x0F);		 // mask with 11110000 and shift right 4 times
	// info.start_of_sequence = ((is_start & 0x08) >> 3); // 00001000
	// info.end_of_sequence = ((is_end & 0x04) >> 2);   // 00000100
	// info.initial_packet = ((is_init & 0x02) >> 1);	// 00000010
	// info.do_not_continue = (is_do_not_continue & 0x01); // 00000001
	info.packet_type = type;
	info.start_of_sequence = is_start;
	info.end_of_sequence = is_end;
	info.initial_packet = is_init;
	info.do_not_continue = is_do_not_continue;
	return info;
}

transport_header_t transport_header_fromfields(uint16_t seq_num, uint8_t type, uint8_t is_start, uint8_t is_end, uint8_t is_init, uint8_t is_do_not_continue){
	transport_header_t header;
	// TMTC specifies MSB first, hence flipping.
	#if little_endian
		flip_endian(cast_asptr(seq_num), PACKET_SEQUENCE_LEN);
	#endif
	header.as_struct.sequence_number = seq_num;
	header.as_struct.info = transport_info_fromfields(type, is_start, is_end, is_init, is_do_not_continue);	
	return header;
}

uint32_t packet_write_transport_header_to_buffer(uint16_t buffer_block_num, transport_header_t transport_header){
	return buffer_write_reserved(buffer_block_num, PACKET_SEQUENCE_START_INDEX, transport_header.as_bytes, TRANSPORT_LEN);
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
	packet_write_transport_header_to_buffer(seq_num, current_header);
	seq_num++;
	for(seq_num ; seq_num < last_block_num; seq_num++){
		block_num = seq_num % BUFFER_BLOCKS;
		// debug_printf("prepping transport for block %d", seq_num);
		// make transport header
		current_header = transport_header_fromfields(seq_num, PACKET_TYPE_DAT, 0, 0, 0, 0);
		packet_write_transport_header_to_buffer(seq_num, current_header);
	}
	// for loop exits when seq_num = last_block_num
	current_header = transport_header_fromfields(seq_num, PACKET_TYPE_DAT, 0, 1, 0, 0);
	packet_write_transport_header_to_buffer(seq_num, current_header);
	return seq_num++;
}

/* Reading packets */
