

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../driver/fram.h"
#include "../utility/debug.h"

uint8_t dummy_fram[FRAM_SIZE];

bool FRAM_selfTest(void){

}

void FRAM_write(uint32_t address, uint8_t *data, uint32_t length){
	static bool have_written;
	if (!have_written){
		debug_printf("FRAM.C: dummy_fram start address: %p", &dummy_fram);
		have_written = true;
	}
	
	// debug_printf("FRAM.C: writing to FRAM address: ")
	for(uint32_t i = 0; i<length; i++){
		dummy_fram[i+address] = data[i];
	}
}

void FRAM_read(uint32_t address, uint8_t *data, uint32_t length){
	for(uint32_t i = 0; i < length; i++){
		data[i] = dummy_fram[i+address];
	}
}
