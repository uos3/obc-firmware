#include <stdint.h>

#include "packet_base.h"


uint32_t packet_get_data_len(Packet* packet){
    return packet->length - PACKET_HASH_LEN;
}
