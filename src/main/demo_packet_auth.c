#include <string.h>
#include <stdio.h>

#include "../utility/debug.h"
#include "../mission/packet_auth.h"

char output[256];

int main(){
	debug_init();
	debug_clear();
	debug_print("=== Packet auth demo ===");

	uint8_t truth;
	// uint8_t data[255-PACKET_HASH_LEN] = "Did you ever hear the Tragedy of Darth Plagueis the wise? I thought not. It's not a story the Jedi would tell you. It's a Sith legend. Darth Plagueis was a Dark Lord of the Sith, so powerful and so wise he could use the Force to influence the midichlorians to create life... He had such a knowledge of the dark side that he could even keep the ones he cared about from dying. The dark side of the Force is a pathway to many abilities some consider to be unnatural. He became so powerful... the only thing he was afraid of was losing his power, which eventually, of course, he did. Unfortunately, he taught his apprentice everything he knew, then his apprentice killed him in his sleep. It's ironic he could save others from death, but not himself";
	uint8_t data[] = "hello world!";
	debug_print("Original data:");

	debug_hex(data, sizeof(data));
	debug_print("setting up packet");

	Packet packet;
	packet.length = sizeof(data) + PACKET_HASH_LEN;
	memcpy(packet.data, data, packet_get_data_len(&packet));

	debug_print("producing initial hash");
	packet_hasher(&packet);

	debug_print("testing the authentication function");
	truth = packet_is_authentic(&packet);
	sprintf(output, "compare result: %u", truth);
	debug_print(output);

	debug_print("=== end demo ===");	
	debug_end();
}