/**
 * @ingroup gnulinux
 * @ingroup fram
 *
 * @file fram.c
 *
 * @{
 */

#include "board.h"
#include "../fram.h"

#include <string.h>

static uint8_t fram_data[FRAM_SIZE] = { 0 };

void FRAM_init(void) {}

bool FRAM_selfTest(void)
{
  /* For behaviour-parity with embedded implementations */
  //Fram_init();

  return true;
}

void FRAM_write(uint32_t address, uint8_t *data, uint32_t length)
{
	uint32_t i = 0;

  while(i<length)
  {
	/* Mask address to emulate hardware overflow behaviour */
  	fram_data[((address + i++) & FRAM_MAX_ADDRESS)] = *(data++);
  }
}

void FRAM_read(uint32_t address, uint8_t *data, uint32_t length)
{
	uint32_t i = 0;
	
  while(i<length)
  {
	/* Mask address to emulate hardware overflow behaviour */
  	*(data++) = fram_data[((address + i++) & FRAM_MAX_ADDRESS)];
  }
}

/**
 * @}
 */