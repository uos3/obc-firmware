/**
 * @ingroup crc
 *
 * @file crc16.c
 *
 * @{
 */

#include "../firmware.h"

void Packet_crc16(uint8_t *input, uint32_t length, uint16_t *output)
{
  uint32_t i;
  uint16_t j;
 
  *output = 0xFFFF;
  
  for (i=0; i<length; i++)
  {
    *output = (uint16_t)(*output ^ ((uint16_t)input[i] << 8));
    for (j=0; j<8; j++)
    {
      if (*output & 0x8000)
        *output = (uint16_t)(*output << 1) ^ 0x1021;
      else
        *output = (uint16_t)(*output << 1);
    }
  }
}

/**
 * @}
 */