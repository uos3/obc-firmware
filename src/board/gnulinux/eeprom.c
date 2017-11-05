/**
 * @ingroup gnulinux
 * @ingroup eeprom
 *
 * @file eeprom.c
 *
 * @{
 */

#include "board.h"
#include "../eeprom.h"

void EEPROM_init(void)
{
  
}

bool EEPROM_selfTest(void)
{
  return true;
}

void EEPROM_write(uint32_t address, uint32_t *data, uint32_t length)
{
	(void) address;
	(void) data;
	(void) length;
	// TODO
}

void EEPROM_read(uint32_t address, uint32_t *data, uint32_t length)
{
	(void) address;
	(void) data;
	(void) length;
	// TODO
}

/**
 * @}
 */