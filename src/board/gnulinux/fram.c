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

/* 1Mbit (128KB) FM25V10 */
#define FRAM_MAX_ADDRESS	0x1FFFF

/* 4Mbit (512KB) CY15B104Q */
//#define FRAM_MAX_ADDRESS	0x7FFFF

static uint8_t fram_data[FRAM_MAX_ADDRESS + 1] = { 0 };

void Fram_init(void) {}

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