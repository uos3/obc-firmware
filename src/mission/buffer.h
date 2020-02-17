#ifndef BUFFER
#define BUFFER

#include <stdint.h>
#include "../component/fram.h"

#define BLOCK_SIZE 256
/* block 0 is reserved for semi-non volitile. CRC is included in the packet at the end*/
#define DATA_START_INDEX 20
/* TMTC v4.6 defines first application byte as index 20 (not 19, you can't count.) */
#define BUFFER_STATUS_MAX_LEN 4
/* used as an offset for writing status to the buffer. */
#define BUFFER_BLOCKS FRAM_SIZE/BLOCK_SIZE

typedef struct buffer_status_t
{
    uint8_t buffer_init;
    /* the address in the block of the next writable location */
    uint8_t current_block_position;
    /* the block's index in the FRAM */
    uint32_t current_block_address;
    /* the block ready for the next transmit */
    uint32_t transmit_block_address;
} buffer_status_t;

buffer_status_t buffer_status;

uint8_t buffer_retrieve_status();

void _store_buffer_status(void);

uint32_t buffer_write_next(uint8_t *data, uint32_t data_len);

void buffer_init(void);

void _buffer_overwrite_table();

uint8_t buffer_retrieve_next_transmit(uint8_t *block_buffer, uint8_t *block_length);

void buffer_free_block(uint32_t block_address);

#endif /* BUFFER */