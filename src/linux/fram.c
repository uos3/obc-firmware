

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../driver/fram.h"

static uint8_t dummy_fram[FRAM_SIZE];

bool FRAM_selfTest(void);

void FRAM_write(uint32_t address, uint8_t *data, uint32_t length){
	for(uint32_t i = 0; i<length; i++){
		dummy_fram[i+address] = data[i];
	}
}

void FRAM_read(uint32_t address, uint8_t *data, uint32_t length){
	for(uint32_t i = 0; i < length; i++){
		data[i] = dummy_fram[i+address];
	}
}
