#ifndef PACKET_BASE
#define PACKET_BASE

#include <stdint.h>

// could define this as `BLOCK_SIZE - 1` but that would introduce a dependancy.
#define PACKET_LEN 255

/* packet structure definitions from TMTC_20191127_v4.6 */
#define PACKET_HASH_LEN 16
#define PACKET_HASH_START_INDEX 1 // for use with the buffer. 

#define PACKET_SEQUENCE_TYPE uint16_t
#define PACKET_SEQUENCE_LEN sizeof(PACKET_SEQUENCE_TYPE)
#define PACKET_SEQUENCE_START_INDEX (PACKET_HASH_START_INDEX+PACKET_HASH_LEN) // expands to 17
#define TRANSPORT_INFO_LEN 1
#define TRANSPORT_INFO_START_INDEX (PACKET_SEQUENCE_START_INDEX+PACKET_SEQUENCE_LEN) // expands to 19
#define TRANSPORT_LEN PACKET_SEQUENCE_LEN+TRANSPORT_INFO_LEN // expands to 3

typedef struct Packet{
    uint8_t length;
    uint8_t packet_buffer[PACKET_LEN];
    uint8_t* auth_start;
    uint8_t* transport_start;
    uint8_t* application_start;
}Packet;

uint32_t packet_get_data_len(Packet* packet);

#endif /* PACKET_BASE */
