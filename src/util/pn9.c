/**
 * @ingroup pn9
 *
 * @file pn9.c
 *
 * @{
 */

#include "../firmware.h"

void Util_pn9(uint8_t *buffer, uint32_t preroll, uint32_t length)
{
  /* Uses CC1120 PN9 Sequence */
  uint16_t state;
  uint16_t j;

  if(length == 0)
    return;

  state = 0x01FF;

  if(preroll == 0)
  {
    *buffer++ ^= 0xFF;
    length--;
  }
  else
  {
    while(preroll--)
    {
      for(j=0; j<8; j++)
      {
        state = (uint16_t)((state >> 1) + ((((state & 0x01) ^ (state >> 5)) & 0x01) << 8));
      }
    }
    *buffer++ ^= (uint8_t)state;
    length--;
  }

  while(length--)
  {
    for(j=0; j<8; j++)
    {
      state = (uint16_t)((state >> 1) + ((((state & 0x01) ^ (state >> 5)) & 0x01) << 8));
    }

    *buffer++ ^= (uint8_t)state;
  }
}

/**
 * @}
 */