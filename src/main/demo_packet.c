#include "../utility/debug.h"

#include "../mission/packet.h"
#include "../mission/packet_transport.h"
#include "../mission/buffer.h"


void packet_transmit_buffer(){
	uint16_t block_num, first_block_num, last_block_num;
	uint8_t block_buffer[BLOCK_SIZE];
	uint8_t length;
	first_block_num = buffer_status.transmit_block_address;
	last_block_num = buffer_status.current_block_address;
	for (block_num = first_block_num; block_num <= last_block_num; block_num++){
		// read the packet
		buffer_retrieve_block(block_num, block_buffer, &length);
		// send the packet to the 'TX'
		// debug_hex(&block_buffer[1], APPLICATION_START_INDEX);
		debug_printl(&block_buffer[APPLICATION_START_INDEX], length-BUFFER_DATA_START_INDEX);
	}
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
	packet_transmit_buffer();

	debug_end();
	return 0;
}
