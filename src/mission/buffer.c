#include <inttypes.h>

#include "../component/fram.h"
#include "buffer.h"

buffer_status current_buffer_status;

void _store_buffer_status(void){
    /*
         TODO - implement buffer status in FRAM such that it can be read
         ideally, we can avoid overwriting data 
    */
    
}


uint32_t buffer_write_to_current(uint8_t* data, uint32_t data_len){
    /*
        if length of data can fit in the block
        If a buffer slot has a sequence number, do not use.
    */
    uint32_t address;
    if (current_buffer_status.buffer_init  <= (uint8_t) 0x01){
        #ifdef DEBUG_MODE
            UART_puts(UART_INTERFACE, "BUFFER.C: Buffer written to before init run. Initiating now...\r\n");
        #endif
        buffer_init();
    }
    /*
        check buffer length against data length
    */
    if (BLOCK_SIZE - current_buffer_status.current_pos_in_block < data_len){
        /*
            address is block No * 256, as block size is 256. Can also use bit shifting
            address is then offset by the current pos in block
        */
        #ifdef DEBUG_MODE
            UART_puts(UART_INTERFACE, "BUFFER.C: writing less than 1 full block to buffer\r\n");
        #endif
        address = (uint32_t) current_buffer_status.current_block * BLOCK_SIZE;
        address+= (uint32_t) current_buffer_status.current_pos_in_block;
        FRAM_write(address, data, data_len);
        // Need to update current position in block. Already know data fits.
        current_buffer_status.current_pos_in_block += data_len;
        return data_len;
    }
    else{
        /*
            Can only write what's free in the block. Doing anything remotely 
            intelligent here would prevent higher level (e.g. packet) decision
            making from occuring.
        */
        return -1;
    }
}


void buffer_init(void){
    /*
        fill in the current_buffer_status
        the first block is reserved for TBD. Too small for full table, but big enough
        to hold buffer status in case of restart. 

        Buffer addresses are sequence numbers.
    */
    current_buffer_status.buffer_init = (uint8_t) 0xFF;
    current_buffer_status.current_block = (uint32_t) 0x01;
    current_buffer_status.current_pos_in_block = (uint32_t) DATA_START_INDEX;
}

