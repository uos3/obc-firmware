#include "crc16.h"

uint16_t CRC16_CCITT(uint8_t *buffer, uint32_t length)
{
  uint32_t i, j;
  uint16_t crc;
 
  crc = 0xFFFF;
  
  for (i=0; i<length; i++)
  {
    crc = crc ^ ((uint16_t)buffer[i] << 8);
    for (j=0; j<8; j++)
    {
      if (crc & 0x8000)
        crc = (crc << 1) ^ 0x1021;
      else
        crc <<= 1;
    }
  }
 
  return crc;
}