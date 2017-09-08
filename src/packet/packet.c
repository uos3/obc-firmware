#include "../firmware.h"

void packet_scramble_xor(uint8_t *buffer, uint32_t length)
{
  /* Uses CC1120 PN9 Sequence */

  uint16_t state = 0xFF;
  uint32_t i;

  if(length < 1)
    return;

  state = 0x00FF;
  buffer[0] ^= (uint8_t)state;

  for(i=1; i<length; i++)
  {
    state = (uint16_t)((state >> 1) + ((((state & 0x01) ^ (state >> 5)) & 0x01) << 8));

    buffer[i] ^= (uint8_t)state;
  }
}

void packet_interleave(uint8_t *in, uint8_t *out, uint32_t x, uint32_t y)
{
  (void) in;
  (void) out;

  (void) x;
  (void) y;
}