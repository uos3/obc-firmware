/* @file interleave.c
 *
 * Packet Interleaving Functions
 *
 * @author Phil Crump
 *
 * @ingroup packet
 * @{
 */

#include "../firmware.h"

/**
 * @brief Interleave (transpose) a 32x32 bit buffer.
 *
 * @description Length of input must be 1024 bits (128 bytes).
 *
 * @param buffer Pointer to the buffer to be interleaved.
 */
void Packet_interleave_32x32(uint8_t *buffer)
{
   uint32_t i, j;
   uint8_t output[128] = { 0 };

   for(i=0;i<1024;i++)
   {
      j = (((i & 0x1f) << 5) + (i >> 5));
      
      if((i & 0x07) >= (j & 0x07))
      {
         output[j>>3] = (uint8_t)(output[j>>3] | ((buffer[i>>3] & (0x80 >> (i & 0x07))) << ((i & 0x07) - (j & 0x07))));
      }
      else
      {
         output[j>>3] = (uint8_t)(output[j>>3] | ((buffer[i>>3] & (0x80 >> (i & 0x07))) >> ((j & 0x07) - (i & 0x07))));
      }
   }

   memcpy(buffer, output, 128);
}

/**
 * @brief Interleave (transpose) a 64x64 bit buffer.
 *
 * @description Length of input must be 4096 bits (512 bytes).
 *
 * @param buffer Pointer to the buffer to be interleaved.
 */
void Packet_interleave_64x64(uint8_t *buffer)
{
   uint32_t i, j;
   uint8_t output[512] = { 0 };

   for(i=0;i<4096;i++)
   {
      j = (((i & 0x3f) << 6) + (i >> 6));
      
      if((i & 0x07) >= (j & 0x07))
      {
         output[j>>3] = (uint8_t)(output[j>>3] | ((buffer[i>>3] & (0x80 >> (i & 0x07))) << ((i & 0x07) - (j & 0x07))));
      }
      else
      {
         output[j>>3] = (uint8_t)(output[j>>3] | ((buffer[i>>3] & (0x80 >> (i & 0x07))) >> ((j & 0x07) - (i & 0x07))));
      }
   }

   memcpy(buffer, output, 512);
}

/**
 * @}
 */