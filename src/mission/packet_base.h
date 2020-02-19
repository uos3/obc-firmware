#ifndef PACKET_BASE
#define PACKET_BASE

#include <stdint.h>

#define PACKET_LEN 255
#define PACKET_HASH_LEN 16

typedef struct Packet{
    uint8_t length;
    uint8_t hash[PACKET_HASH_LEN];
    uint8_t data[PACKET_LEN - PACKET_HASH_LEN];
}Packet;

uint32_t packet_get_data_len(Packet* packet);

#endif /* PACKET_BASE */
