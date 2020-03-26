#ifndef PACKET_BASE_H
#define PACKET_BASE_H

#include <stdint.h>

// could define this as `BLOCK_SIZE - 1` but that would introduce a dependancy.
#define PACKET_LEN 255

/* packet structure definitions from TMTC_20191127_v4.6 */
#define AUTH_LEN 16
#define AUTH_START_INDEX 1 // for use with the buffer. 

#define TRANSPORT_SEQUENCE_TYPE uint16_t
#define TRANSPORT_SEQUENCE_LEN sizeof(TRANSPORT_SEQUENCE_TYPE)
#define TRANSPORT_SEQUENCE_START_INDEX (AUTH_START_INDEX+AUTH_LEN) // expands to 17
#define TRANSPORT_INFO_LEN 1
#define TRANSPORT_INFO_START_INDEX (TRANSPORT_SEQUENCE_START_INDEX+TRANSPORT_SEQUENCE_LEN) // expands to 19
#define TRANSPORT_LEN TRANSPORT_SEQUENCE_LEN+TRANSPORT_INFO_LEN // expands to 3

#define APPLICATION_START_INDEX TRANSPORT_SEQUENCE_START_INDEX+TRANSPORT_LEN // expands to 20?

#endif /* PACKET_BASE_H */
