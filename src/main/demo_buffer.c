/*
	Author: Richard A
	Last modified: 2020 02 17
	Status: Indev
*/
#define DEBUG_MODE

#include <stdio.h>

#include "../driver/board.h"
#include "../driver/wdt.h"
#include "../driver/watchdog_ext.h"
#include "../component/fram.h"
#include "../mission/buffer.h"
#include "../utility/debug.h"

char output[256];


void buffer_status_print(){
	sprintf(output, "demo: buffer status: %u, cba %u, cbp %u, tba %u",
			buffer_status.buffer_init,
			buffer_status.current_block_address,
			buffer_status.current_block_position,
			buffer_status.transmit_block_address);
	debug_print(output);
}


int main(){
	Board_init();
	debug_init();
	debug_clear();
	enable_watchdog_kick();
	watchdog_update = 0xFF;

	debug_print("=== DEMO buffer ===");
	debug_print("reads the non-volitile status, write some data, read it back");

	// _buffer_overwrite_table();
	uint8_t data_to_write[] = "some data to write ";
	// truncated data to remove null terminator. Going to add more data later to prove functionality. 
	uint32_t data_len = 19;

	uint8_t block_buffer[256];
	uint8_t read_length;
	// check the status before any writing has occured
	debug_print("demo: retrieving the status stored");
	buffer_retrieve_status();
	buffer_status_print();

	// write to buffer
	debug_print("demo: writing to buffer. Buffer should init if the above is not performed");
	buffer_write_next(data_to_write, data_len);
	// status should have been updated
	debug_print("demo: printing the status post write");
	buffer_status_print();

	// status should have been written to the fram in block 0

	debug_print("demo: retrieving the status from fram post write");
	buffer_retrieve_status();
	buffer_status_print();

	debug_print("demo: reading the block back");
	buffer_retrieve_next_transmit(block_buffer, &read_length);

	block_buffer[255] = "\0";
	sprintf(output, "demo: read length %u", read_length);
	debug_print(output);
	debug_print("demo: contents of buffer:");
	debug_print(block_buffer);


	debug_print("=== end demo ===");
	while(1){
		watchdog_update=0xFF;
	}
}

