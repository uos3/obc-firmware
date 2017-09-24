/**
 * @ingroup pn9
 *
 * @file pn9.c
 *
 * @{
 */

 #include "../firmware.h"

void packet_pn9_xor(uint8_t *buffer, uint32_t length)
{
  /* Uses CC1120 PN9 Sequence */

  uint16_t state = 0xFF;
  uint32_t i, j;

  if(length < 1)
    return;

  state = 0x01FF;
  buffer[0] ^= (uint8_t)state;

  for(i=1; i<length; i++)
  {
    for(j=0; j<8; j++)
    {
      state = (uint16_t)((state >> 1) + ((((state & 0x01) ^ (state >> 5)) & 0x01) << 8));
    }

    buffer[i] ^= (uint8_t)state;
  }
}

/**
 * @}
 */