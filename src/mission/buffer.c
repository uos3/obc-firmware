/*
	New, non-volitile buffer system.
	Author: Richard A
	Last modified: 2020 02 17
	Current status: None overflow functional.
*/


#include <inttypes.h>
#include <stdio.h>

#include "../component/fram.h"
#include "../utility/debug.h"
#include "../utility/byte_plexing.h"


#include "buffer.h"

#ifdef DEBUG_MODE
	char output[128];
#endif

buffer_status_t buffer_status;


void _buffer_store_field(uint32_t* write_address, uint32_t field, uint32_t field_length){
	uint8_t split_field[BUFFER_STATUS_MAX_LEN];
	uint32_t field_start_byte;
	field_start_byte = BUFFER_STATUS_MAX_LEN - field_length;

	data_split_u32_le(field, split_field);

	FRAM_write(*write_address, &split_field[field_start_byte], field_length);
	*write_address += field_length;
	// #ifdef DEBUG_MODE
	// 	sprintf(output, "\tBUFFER.c: status_update: 0x%02X: %lu", *write_address, field);
	// 	debug_print(output);
	// #endif
}

void _buffer_store_status(void){
	/*
		TODO - implement buffer status in FRAM such that it can be read
		ideally, we can avoid overwriting data 
	*/
	uint32_t buffer_write_address, write_length;
	buffer_write_address = 0; write_length = 0;
	// write_length = sizeof(buffer_status.buffer_init);
	// FRAM_write(0x00, buffer_status.buffer_init, write_length);
	// buffer_write_address += write_length;
	// write_length = sizeof(buffer_status.current_block_address);
	// FRAM_write(buffer_write_address, ;
	_buffer_store_field(&buffer_write_address, buffer_status.buffer_init, 1);
	_buffer_store_field(&buffer_write_address, buffer_status.current_block_position, 1);
	_buffer_store_field(&buffer_write_address, buffer_status.current_block_address, 4);
	_buffer_store_field(&buffer_write_address, buffer_status.transmit_block_address, 4);
}


uint8_t buffer_retrieve_status(){
	// uint32_t address, length;
	uint32_t plexed_data;
	uint8_t retrieved_content[BUFFER_STATUS_MAX_LEN];

	FRAM_read(0x00, retrieved_content, 1);
	buffer_status.buffer_init = retrieved_content[0];
	// no combining neccisary, first byte is first byte
	FRAM_read(0x01, retrieved_content, 1);
	// plexed_data = data_combine_u32(retrieved_content);
	buffer_status.current_block_position = retrieved_content[0];

	FRAM_read(0x02, retrieved_content, 4);
	plexed_data = data_combine_u32(retrieved_content);
	buffer_status.current_block_address = plexed_data;


	FRAM_read(0x06, retrieved_content, 4);
	plexed_data = data_combine_u32(retrieved_content);
	buffer_status.transmit_block_address = plexed_data;
	return 0xFF;
}


void _buffer_overwrite_table(){
	buffer_status.buffer_init = (uint8_t) 0xFF;
	buffer_status.current_block_position = (uint8_t) DATA_START_INDEX;
	buffer_status.current_block_address = (uint32_t)0x01;
	buffer_status.transmit_block_address = (uint32_t)0x01;
	_buffer_store_status();
}


void buffer_init(void){
	// 
	buffer_retrieve_status();
	if (buffer_status.buffer_init != 0x00){
		#ifdef DEBUG_MODE
				debug_print("BUFFER.C: Buffer status not found. Overwriting.");
		#endif
		_buffer_overwrite_table();
	}
}

uint32_t buffer_write_next(uint8_t* data, uint32_t data_len){
	/*
	*/
	uint32_t address, block_address;
	uint32_t retval;
	uint8_t block_written_flag[] = {0xFF};

	/*
		if the buffer has not been initialied, do that now. Not optimal to 
		have this in every buffer write, but it is *kind of* neccisary.
	*/
	if (buffer_status.buffer_init != (uint8_t) 0x00){
		// Just checks that init is not 0. Any possible value can be used.
		#ifdef DEBUG_MODE
			debug_print("BUFFER.C: Buffer written to before init run. Initiating now...");
		#endif
		buffer_init();
	}

	if ((BLOCK_SIZE - buffer_status.current_block_position) > data_len)	{
		/*
			address is block No * 256, as block size is 256. Can also use bit shifting
			address is then offset by the current pos in block
		*/
		#ifdef DEBUG_MODE
			debug_print("BUFFER.C: writing less than 1 full block to buffer");
		#endif
		block_address = (uint32_t) buffer_status.current_block_address * BLOCK_SIZE;
		address = block_address + (uint32_t) buffer_status.current_block_position;
		FRAM_write(address, data, data_len);
		// Need to update current position in block. Already know data fits.
		buffer_status.current_block_position += data_len;
		FRAM_write(block_address, &buffer_status.current_block_position, 1);
		retval = data_len;
	}
	else{
		/*
			Can only write what's free in the block. Doing anything remotely 
			intelligent here would prevent higher level (e.g. packet) decision
			making from occuring.
		*/
		retval = 0;
	}
	_buffer_store_status();
	return retval;
}

uint8_t buffer_retrieve_next_transmit(uint8_t block_buffer[], uint8_t *length_in_block){
	// just read the entire block.
	FRAM_read(buffer_status.transmit_block_address*BLOCK_SIZE, block_buffer, BLOCK_SIZE);
	// length should be in the first byte.
	*length_in_block = block_buffer[0];
	return block_buffer[0];
}
