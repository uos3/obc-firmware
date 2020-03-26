/*
    fram demo
    Author: Richard A
    Last modified: 2020 02 13
    Status: indev

    Writes arbitary data to the FRAM, reads it, prints to UART. 
*/
#include <stdlib.h>


#include "../driver/board.h"
#include "../driver/wdt.h"
#include "../driver/watchdog_ext.h"

#include "../component/fram.h"
#include "../component/led.h"

#include "../utility/debug.h"

#include "inc/hw_ints.h"

uint32_t data_length = 14;
uint32_t address = (uint32_t)0x00;

uint32_t write_arbitary_data(){
    char arbitary_data[] = "Hello world!";
    FRAM_write(address, (uint8_t *) &arbitary_data, data_length);
    debug_print(arbitary_data);
    return data_length;
}

uint8_t* read_arbitary_data(){
    uint8_t* arbitary_data;
    arbitary_data = malloc(32* sizeof(uint8_t));

    if (arbitary_data == NULL){
        return NULL;
    }

    FRAM_read(address, arbitary_data, data_length);
    return arbitary_data;
}

void main(){
    uint8_t* arbitary_data;

    Board_init();
    debug_init();
    enable_watchdog_kick();
    
    watchdog_update = 0xFF;

    debug_print("FRAM read/write demo starting");

    debug_print("Writing arbitary data: ");
    write_arbitary_data();

    debug_print("Reading back the arbitary data:");
    arbitary_data = read_arbitary_data();

    debug_print("printing arbitary data");
    debug_print((uint8_t *) arbitary_data);

    debug_print("Demo complete.");
    LED_off(LED_B);

    while(1){
        watchdog_update = 0xFF;
    }
}

