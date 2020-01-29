/**
 * @ingroup uos3-proto
 * @ingroup eeprom
 *
 * @file eeprom.c
 *
 * @{
 */

#include "../driver/board.h"
#include "eeprom.h"

#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"

bool EEPROM_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

	if(EEPROMInit() == EEPROM_INIT_ERROR){return false;}
	return true;
}

bool EEPROM_selfTest(void)
{
  return (EEPROMSizeGet() == EEPROM_SIZE);
}

void EEPROM_write(uint32_t address, uint32_t *data, uint32_t length)
{
	EEPROMProgram(data, address, length);
}

void EEPROM_read(uint32_t address, uint32_t *data, uint32_t length)
{
	EEPROMRead(data, address, length);
}

/**
 * @}
 */