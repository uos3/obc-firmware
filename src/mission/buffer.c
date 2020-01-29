#include <inttypes.h>

#define BLOCK_SIZE 256
/* block 0 is reserved for semi-non volitile. CRC is included in the packet at the end*/
#define DATA_START_INDEX 20
/* TMTC v4.6 defines first application byte as index 20 (not 19, you can't count.) */
typedef struct buffer_status{
    short buffer_init;
    /* the block's index in the FRAM */
    long int current_block; 
    /* the address in the block of the next writable location */
    long int current_block_addr;
} buffer_status;


void buffer_write_next_free(uint8_t* data, int data_len){
    /*
        if length of data can fit in the block
    */

}

