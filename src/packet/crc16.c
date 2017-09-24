/**
 * @ingroup crc
 *
 * @file crc16.c
 *
 * @{
 */

#include "../firmware.h"
#include "crc16.h"

uint16_t Packet_CRC16_CCITT(uint8_t *buffer, uint32_t length)
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