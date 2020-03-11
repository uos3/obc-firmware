/*
	New, non-volitile buffer system.
	Author: Richard A
	Last modified: 2020 03 06
	Current status: functional
	we only have 32k RAM, but maybe some of that could be dedicated to a 
	contignency replacement buffer...
*/


#include <inttypes.h>
#include <stdio.h>

#include "../driver/fram.h"
#include "../utility/debug.h"
#include "../utility/byte_plexing.h"

#include "buffer.h"


#ifdef DEBUG_MODE
	char output[128];
#endif

// buffer_status_t buffer_status;


/* buffer status management */

void buffer_store_status(void){
	FRAM_write(0, buffer_status.as_bytes, sizeof(buffer_status_t));
}


uint8_t buffer_retrieve_status(){
	// uint32_t address, length;
	FRAM_read(0, buffer_status.as_bytes, sizeof(buffer_status_t));
	return 0xFF;
}


void _buffer_overwrite_table(){
	buffer_status.as_struct.buffer_init = (uint8_t) 0xFF;
	buffer_status.as_struct.current_block_position = (uint8_t) BUFFER_DATA_START_INDEX;
	buffer_status.as_struct.current_block_address = (uint32_t)0x01;
	buffer_status.as_struct.transmit_block_address = (uint32_t)0x01;
	buffer_status.as_struct.recieve_block_start = (uint32_t)0x00;
	buffer_status.as_struct.recieve_block_end = (uint32_t) 0x00;
	buffer_store_status();
}


void buffer_init(void){
	buffer_retrieve_status();
	if (buffer_status.as_struct.buffer_init == 0x00){
		#ifdef DEBUG_MODE
				debug_print("BUFFER.C: Buffer status not found. Overwriting.");
		#endif
		_buffer_overwrite_table();
	}
}

void buffer_init_check(){
	/*
		if the buffer has not been initialied, do that now. Not optimal to 
		have this in every buffer write, but it is *kind of* neccisary.
	*/
	if (buffer_status.as_struct.buffer_init == (uint8_t)0x00)
	{
		// Just checks that init is not 0. Any possible value can be used.
		#ifdef DEBUG_MODE
			debug_print("BUFFER.C: Buffer written to before init run. Initiating now...");
		#endif
		buffer_init();
	}
}


/* buffer internal writing functions */


uint32_t _buffer_write(uint8_t* data, uint32_t data_len){
	// DO NOT USE DIRECTLY OUTSIDE OF BUFFER.C
	uint32_t block_address, address;
	block_address = buffer_status.as_struct.current_block_address * BLOCK_SIZE;
	address = block_address + (uint32_t)buffer_status.as_struct.current_block_position;
	FRAM_write(address, data, data_len);
	buffer_status.as_struct.current_block_position += data_len;
	FRAM_write(block_address, &buffer_status.as_struct.current_block_position, 1);
	buffer_store_status();
	return data_len;
}


uint32_t buffer_get_free_length(){
	return BLOCK_SIZE - buffer_status.as_struct.current_block_position-1;
}


uint32_t buffer_get_block_used_length(uint32_t block_number){
	uint8_t used_length;
	FRAM_read(buffer_status.as_struct.current_block_address*BLOCK_SIZE, &used_length, 1);
	return (uint32_t) used_length;
}


void buffer_increment_block(){
	// dumb way of doing it. Oldest data gets overwritten first
	// uint32_t block_address = buffer_status.current_block_address;
	// because i'm a dumby dumb and can't figure out how % operator works
	buffer_status.as_struct.current_block_address += 1;
	if (buffer_status.as_struct.current_block_address == 0){
		// if it's zero, don't.
		buffer_status.as_struct.current_block_address += 1;
	}
	buffer_status.as_struct.current_block_position = BUFFER_DATA_START_INDEX;
	buffer_store_status();
}


/* data writing functions */


uint32_t buffer_write_reserved(uint32_t block_number, uint8_t write_start_position, uint8_t data[], uint32_t data_len){
	/* write protections */
	if (block_number == 0){
		#ifdef DEBUG_MODE
			debug_print("BUFFER.C: buffer reserved write attempted to write to reserved block");
		#endif
		return 0;
	}
	if ((data_len + write_start_position) > BUFFER_DATA_START_INDEX){
		// if trying to use this function to write over the data, don't
		// >= as we start at index 1, not 0. functionally, D_S_I is the same as the header length.
		#ifdef DEBUG_MODE
			debug_print("BUFFER.C: buffer reserved write greater than reserved area detected");
		#endif
		return 0;
	}
	if (write_start_position == 0){
		#ifdef DEBUG_MODE
			debug_print("BUFFER.C: buffer reserved write with start position of 0; can't overwrite length field");
		#endif
		return 0;
	}
	if (buffer_get_block_used_length(block_number) < BUFFER_DATA_START_INDEX){
		// block lengths 0 -> 20 can be used to signify anything in principle, so we don't want to write to them
		#ifdef DEBUG_MODE
			debug_print("BUFFER.c buffer reserved write attempted to write to non-typical block");
		#endif
		return 0;
	}
	/* /write protections */
	// implicit else
	uint32_t start_address;
	start_address = block_number * BLOCK_SIZE + write_start_position;
	FRAM_write(start_address, data, data_len);
	return data_len;
}


uint32_t buffer_write_next(uint8_t* data, uint32_t data_len){
	uint32_t address, block_address;
	uint32_t retval;
	uint32_t free_space_len;

	buffer_init_check();

	free_space_len = buffer_get_free_length();
	while (data_len > free_space_len){
		// write up to data split index
		retval = _buffer_write(data, free_space_len);
		// move to the next block
		buffer_increment_block();
		// reduce the length to write by that about
		data_len -= retval;
		// #ifdef DEBUG_MODE
		// 	sprintf(output, "BUFFER.C: data writing has occured: fsp, %u retval %u, datalen %u to CBA %u", free_space_len, retval, data_len, buffer_status.current_block_address);
		// 	debug_print(output);
		// #endif
		// move up the pointer to the start of the data
		data = &data[retval]; // TODO - replace with pointer arthitmetic & re-test
		// get the free space in the new block
		free_space_len = buffer_get_free_length();
		// #ifdef DEBUG_MODE
		// 	debug_print("BUFFER.C: writing a full block and incrementing");
		// #endif
	}
	if (data_len < free_space_len){
		/*
			address is block No * 256, as block size is 256. Can also use bit shifting
			address is then offset by the current pos in block
		*/
		retval = _buffer_write(data, data_len);
		// #ifdef DEBUG_MODE
		// 	sprintf(output, "BUFFER.C: last data write in series: fsp, %u retval %u, datalen %u to CBA %u", free_space_len, retval, data_len, buffer_status.current_block_address);
		// 	debug_print(output);
		// #endif
			// Need to update current position in block. Already know data fits.
	}
	buffer_store_status();
	return retval;
}

void buffer_retrieve_block(uint16_t block_num, uint8_t block_buffer[], uint8_t *length_in_block){
	// just read the entire block.
	FRAM_read(block_num*BLOCK_SIZE, block_buffer, BLOCK_SIZE);
	// length should be in the first byte.
	*length_in_block = block_buffer[0];
	// return block_buffer[0];
}

void buffer_free_block(uint32_t block_address){
	uint8_t free_data[] = {0x00};
	FRAM_write(block_address*BLOCK_SIZE, free_data, 1);
}
