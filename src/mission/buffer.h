#include <stdint.h>

#ifndef BUFFER
#define BUFFER

#define BLOCK_SIZE 256
/* block 0 is reserved for semi-non volitile. CRC is included in the packet at the end*/
#define DATA_START_INDEX 20
/* TMTC v4.6 defines first application byte as index 20 (not 19, you can't count.) */
typedef struct buffer_status{
    uint8_t buffer_init;
    /* the address in the block of the next writable location */
    uint8_t current_pos_in_block;
    /* the block's index in the FRAM */
    uint32_t current_block; 
} buffer_status;

#endif /* BUFFER */