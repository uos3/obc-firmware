#include <string.h>
#include <stdlib.h>
#include "../utility/debug.h"
#include "../utility/byte_plexing.h"
#include "../driver/fram.h"

#include "../mission/packet.h"
#include "../mission/packet_transport.h"
#include "../mission/buffer.h"


void packet_transmit_buffer(){
	uint16_t block_num, first_block_num, last_block_num;
	uint8_t block_buffer[BLOCK_SIZE];
	uint8_t length;
	first_block_num = buffer_status.as_struct.transmit_block_address;
	last_block_num = buffer_status.as_struct.current_block_address;
	for (block_num = first_block_num; block_num <= last_block_num; block_num++){
		// read the packet
		buffer_retrieve_block(block_num, block_buffer, &length);
		// send the packet to the 'TX'
		// debug_hex(&block_buffer[1], APPLICATION_START_INDEX);
		debug_printl(&block_buffer[APPLICATION_START_INDEX], length-BUFFER_DATA_START_INDEX);
	}
}

void print_transport_header(transport_header_struct header){
	// debug_print("printing transport header");
	debug_print("| seqeunce num\t| type\t| S\t| E\t| I\t| D\t|");
	debug_printf("| %12d\t| %2d\t| %1d\t| %1d\t| %1d\t| %1d\t|", 
		header.sequence_number, 
		header.info.packet_type, 
		header.info.start_of_sequence,
		header.info.end_of_sequence,
		header.info.initial_packet,
		header.info.do_not_continue
	);

}

void print_app_header(app_header_struct_t header){
	debug_print("| application length\t| whofor\t| U\t| R\t| C\t| N\t|");
	debug_printf("| % 18u\t| % 6d\t| %1d\t| %1d\t| %1d\t| %1d\t|",
		header.length,
		header.info.unfinished,
		header.info.ret,
		header.info.confirm,
		header.info.now
	);
}

long get_mem_offset(void* p1, void* p2){
	long offset;
	offset = (long) p2 - (long) p1;
	return offset;
}


void print_packet(packet_typed_t* packet){
	long offset;
	debug_printf("packet at %p", packet);
	// debug_hex(packet->as_bytes, sizeof(packet_typed_t));
	debug_disable_newline();
	for (int i = 0; i < BUFFER_DATA_START_INDEX; i++){
		if (i >= AUTH_START_INDEX && i < TRANSPORT_SEQUENCE_START_INDEX){
			debug_print("---- ");
		}
		else{
			debug_hex((uint8_t*) &packet->as_bytes[i], 1);
		}
	}
	debug_enable_newline();
	debug_print("");

	// length
	offset = get_mem_offset(packet, &packet->as_struct.length);
	debug_printf("length, offset: %ld ---", offset);
	debug_printf("| % 3d\t|", packet->as_struct.length);

	// hash
	offset = get_mem_offset(packet, &packet->as_struct.hash.as_bytes);
	debug_printf("hash, offset: %ld ---", offset);
	// debug_hex(packet->as_struct.hash.bytes, PACKET_HASH_LEN);
	
	// transport header
	offset = get_mem_offset(packet, &packet->as_struct.transport_header);
	debug_printf("transport header, offset: %ld ---", offset);
	print_transport_header(packet->as_struct.transport_header);
}


int main(){
	debug_init();
	debug_print("=== packet stack demo ===");

	uint8_t lorem_ipsum[] = "Did you ever hear the Tragedy of Darth Plagueis the wise? I thought not. It's not a story the Jedi would tell you. It's a Sith legend. Darth Plagueis was a Dark Lord of the Sith, so powerful and so wise he could use the Force to influence the midichlorians to create life... He had such a knowledge of the dark side that he could even keep the ones he cared about from dying. The dark side of the Force is a pathway to many abilities some consider to be unnatural. He became so powerful... the only thing he was afraid of was losing his power, which eventually, of course, he did. Unfortunately, he taught his apprentice everything he knew, then his apprentice killed him in his sleep. It's ironic he could save others from death, but not himself";
	// uint32_t lorem_ipsum_len = 747;
	// uint8_t lorem_ipsum[] = "hello world!";
	uint32_t lorem_ipsum_len = sizeof(lorem_ipsum);
	_buffer_overwrite_table();
	store_payload_data(PACKET_TYPE_DAT, (uint8_t*) &lorem_ipsum, lorem_ipsum_len);
	// add the transport layer to all completed packets
	packet_prep_transport();
	// packet_transmit_buffer();
	debug_printf("packet struct length: %ld", sizeof(packet_typed_struct));
	// read transport demo
	// treating the stored data as recieved data.
	// section is more important for ground station than it is for cubey, as more assumptions can be made
	buffer_status.as_struct.recieve_block_start = buffer_status.as_struct.transmit_block_address;
	buffer_status.as_struct.recieve_block_end = buffer_status.as_struct.current_block_address+1;
	debug_printf("Read test: transmit address: %d, cba: %d", buffer_status.as_struct.transmit_block_address, buffer_status.as_struct.current_block_address);

	packet_typed_t current_packet;
	uint8_t readin_buffer[BLOCK_SIZE];
	uint8_t length;
	for (int block_num = buffer_status.as_struct.recieve_block_start; block_num<buffer_status.as_struct.recieve_block_end; block_num++){
		// read block in, store in packet struct
		packet_retrieve_from_buffer(block_num, &current_packet, &length);
		// check that start of sequence is infact start of sequence
		debug_printf("\r\n=== printing packet number %d ===", block_num);
		print_packet(&current_packet);
	}

	// read test, can we recover the data.

	uint8_t* data;
	uint32_t data_length;
	uint8_t app_position;
	app_position = 0;
	app_header_t first_app_header;
	// first_app_header = (buffer_status.recieve_start_block, app_position);
	// should just be able to print it?
	debug_printf("length: %u", data_length);
	debug_enable_newline();
	debug_printl(data, data_length);
	app_header_t apph;
	debug_hex(data, sizeof(app_header_t));
	memcpy(apph.as_bytes, data, sizeof(app_header_t));
	print_app_header(apph.as_struct);
	free(data);

	debug_print("=== end demo ===");
	debug_end();
	return 0;
}
