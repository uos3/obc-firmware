#ifndef BUFFER
#define BUFFER

#include <stdint.h>
#include "../component/fram.h"

/* block 0 is reserved for semi-non volitile. CRC is included in the packet at the end*/
#define BLOCK_SIZE 256
/*
    TMTC v4.6 defines first application byte as index 20 (not 19, you can't count.)
    1 for length + 16 for hash + 3 for transport
*/
#define BUFFER_DATA_START_INDEX 20
/* used as an offset for writing status to the buffer. */
#define BUFFER_STATUS_MAX_LEN 4
#define BUFFER_BLOCKS FRAM_SIZE/BLOCK_SIZE

#pragma pack(1)
typedef struct buffer_status_struct
{
    uint8_t buffer_init;
    /* the address in the block of the next writable location */
    uint8_t current_block_position;
    /* the block's index in the FRAM */
    uint32_t current_block_address;
    /* the first block to transmit */
    uint32_t transmit_block_address;
    /* */
    uint32_t recieve_block_start;
    uint32_t recieve_block_end;
} buffer_status_struct;

typedef union buffer_status_t{
    buffer_status_struct as_struct;
    uint8_t as_bytes[sizeof(buffer_status_struct)];
} buffer_status_t;

buffer_status_t buffer_status;

uint8_t buffer_retrieve_status();

uint32_t buffer_write_next(uint8_t *data, uint32_t data_len);

uint32_t buffer_write_reserved(uint32_t block_number, uint8_t write_start_position, uint8_t data[], uint32_t data_len);

void buffer_init(void);

void _buffer_overwrite_table();

void buffer_retrieve_block(uint16_t block_num, uint8_t *block_buffer, uint8_t *block_length);

void buffer_free_block(uint32_t block_address);

void buffer_increment_block();

#endif /* BUFFER */