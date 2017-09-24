/**
 * @file crc16.c
 * @ingroup packet
 * @{
 */

#include "crc16.h"

/**
 * Calculate the 16-bit CRC of the input buffer,
 * using the CRC16-CCITT standard.
 *
 * @param buffer The input buffer to calculated
 * @param length The length of the input buffer
 * @returns The 16bit CRC of the input buffer
 */
uint16_t CRC16_CCITT(uint8_t *buffer, uint32_t length)
{
  uint32_t i;
  uint16_t j, crc;
 
  crc = 0xFFFF;
  
  for (i=0; i<length; i++)
  {
    crc = (uint16_t)(crc ^ ((uint16_t)buffer[i] << 8));
    for (j=0; j<8; j++)
    {
      if (crc & 0x8000)
        crc = (uint16_t)(crc << 1) ^ 0x1021;
      else
        crc = (uint16_t)(crc << 1);
    }
  }
 
  return crc;
}

/**
 * @}
 */