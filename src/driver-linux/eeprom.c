#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../driver/eeprom.h"

/* 2KB TM4C123GH6PM */
#define EEPROM_SIZE	0x800

#define EEPROM_MAX_ADDRESS (EEPROM_SIZE - 1)

//#define EEPROM_MISSION_DATA_ADDRESS 0x00000001
//#define EEPROM_ADM_DEPLOY_ATTEMPTS 0x00000002
//#define EEPROM_CONFIGURATION_DATA_ADDRESS 0x0100

bool EEPROM_init(void){
	return true;
}

bool EEPROM_selfTest(void){
	return true;
}

void EEPROM_write(uint32_t address, uint32_t *data, uint32_t length){
	
}

void EEPROM_read(uint32_t address, uint32_t *data, uint32_t length){

}
