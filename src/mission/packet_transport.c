/*
	Provides transport layer functions as defined in TMTC_20191127_v4.6
*/

#include "packet_base.h"
#include "buffer.h"
#include "packet_transport.h"
// #include "../utility/debug.h"
#include "../utility/byte_plexing.h"
// #include <stdio.h>

/* Writing packets */
uint8_t transport_info_asbyte(transport_info_t transport_info){
	uint8_t byte = 0;
	byte |= ((uint8_t) transport_info.packet_type & 0x0F) << 4;
	byte |= ((uint8_t) transport_info.start_of_sequence & 0x01) << 3;
	byte |= ((uint8_t) transport_info.end_of_sequence &0x01) << 2;
	byte |= ((uint8_t) transport_info.initial_packet &0x01) << 1;
	byte |= ((uint8_t) transport_info.do_not_continue &0x01);
	return byte;
}

transport_info_t transport_info_frombyte(uint8_t byte){
	transport_info_t transport_info;
	transport_info.packet_type 			= ((byte & 0xF0) >> 4); // mask with 11110000 and shift right 4 times
	transport_info.start_of_sequence 	= ((byte & 0x08) >> 3); // 00001000
	transport_info.end_of_sequence 		= ((byte & 0x04) >> 2); // 00000100
	transport_info.initial_packet 		= ((byte & 0x02) >> 1); // 00000010
	transport_info.do_not_continue 		=  (byte & 0x01);		// 00000001
	return transport_info;
}

transport_info_t transport_info_fromfields(uint8_t type, uint8_t is_start, uint8_t is_end, uint8_t is_init, uint8_t is_do_not_continue){
	transport_info_t transport_info;
	transport_info.packet_type = (type & 0x0F);		 // mask with 11110000 and shift right 4 times
	transport_info.start_of_sequence = ((is_start & 0x08) >> 3); // 00001000
	transport_info.end_of_sequence = ((is_end & 0x04) >> 2);   // 00000100
	transport_info.initial_packet = ((is_init & 0x02) >> 1);	// 00000010
	transport_info.do_not_continue = (is_do_not_continue & 0x01); // 00000001
	return transport_info;
}

uint32_t packet_write_sequence_to_buffer(uint16_t buffer_block_number, uint16_t sequence_number){
	// uint8_t *sequence_number_list;
	// sequence_number_list = (uint8_t*) &sequence_number; // recast uint16_t as an array of uint8_ts;
	uint8_t data[2];
	// pure cringe, but the uint16_t is LSB first in memory ¯\_(ツ)_/¯
	data[1] = *((uint8_t*) &sequence_number); // contents of the address at the start of sequence number
	data[0] = * (((uint8_t*) &sequence_number)+1); // contents of the second byte of sequence number
	// #ifdef DEBUG_MODE
	// 	char output[256];
	// 	sprintf(output, "addresses of seqnums: %p %p %d", &sequence_number, ((uint8_t*) &sequence_number)+1, sizeof(uint16_t));
	// 	debug_print(output);
	// 	debug_hex(data, 2);
	// #endif
	// return buffer_write_reserved(buffer_block_number, PACKET_SEQUENCE_START_INDEX, (uint8_t*) &sequence_number, PACKET_SEQUENCE_LEN);
	return buffer_write_reserved(buffer_block_number, PACKET_SEQUENCE_START_INDEX, data, PACKET_SEQUENCE_LEN);
}

uint32_t packet_write_transport_info_to_buffer(uint16_t buffer_block_number, transport_info_t transport_info){
	uint8_t data;
	data = transport_info_asbyte(transport_info);
	return buffer_write_reserved(buffer_block_number, TRANSPORT_INFO_START_INDEX, &data, TRANSPORT_INFO_LEN);
}


uint32_t packet_write_transport_header_to_buffer(uint16_t buffer_block_num, transport_header_t transport_header){
	return buffer_write_reserved(buffer_block_num, PACKET_SEQUENCE_START_INDEX, transport_header.as_bytes, TRANSPORT_LEN);
}

transport_header_t transport_header_fromfields(uint16_t seq_num, uint8_t type, uint8_t is_start, uint8_t is_end, uint8_t is_init, uint8_t is_do_not_continue){
	transport_header_t header;
	// TMTC specifies MSB first, hence flipping.
	#if little_endian
		flip_endian(cast_asptr(seq_num), PACKET_SEQUENCE_LEN);
	#endif
	header.as_struct.sequence_number = seq_num;
	header.as_struct.info = transport_info_fromfields(PACKET_TYPE_DAT, 0, 0, 0, 0);	
	return header;
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
	}
	seq_num = first_block_num;
	// first block, start of sequence raised:
	current_header = transport_header_fromfields(seq_num, PACKET_TYPE_DAT, 1, 0, 0, 0);
	packet_write_transport_header_to_buffer(seq_num, current_header);

	for(seq_num ; seq_num < last_block_num; seq_num++){
		block_num = seq_num % BUFFER_BLOCKS;
		// make transport header
		current_header = transport_header_fromfields(seq_num, PACKET_TYPE_DAT, 0, 0, 0, 0);
	}
	// for loop exits when seq_num = last_block_num
	current_header = transport_header_fromfields(seq_num, PACKET_TYPE_DAT, 0, 1, 0, 0);
	packet_write_transport_header_to_buffer(seq_num, current_header);
	return seq_num++;
}

/* Reading packets */
