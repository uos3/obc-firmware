#include "../../firmware.h"
#include "../../test.h"

#define PACKET_LENGTH_32x32 128 // 1024/8

uint8_t packet_test_32x32[PACKET_LENGTH_32x32];
uint8_t packet_reference_32x32[PACKET_LENGTH_32x32];

#define PACKET_LENGTH_64x64 512 // 4096/8

uint8_t packet_test_64x64[PACKET_LENGTH_64x64];
uint8_t packet_reference_64x64[PACKET_LENGTH_64x64];

bool test_interleave(void)
{
	uint32_t i;

  /* Populate packet */
  for(i=0; i<PACKET_LENGTH_32x32; i++)
  {
    packet_reference_32x32[i] = (uint8_t)(Random(255));
    packet_test_32x32[i]      = packet_reference_32x32[i];
  }

  if(TEST_VERBOSE)
  {
    Debug_print("Original packet\r\n");
    buffer_print_bin(packet_test_32x32, PACKET_LENGTH_32x32);
  }

  /* Interleave packet */
  if(TEST_VERBOSE)
  {
    Debug_print("Interleaving packet..\r\n");
  }
  Packet_interleave_32x32(packet_test_32x32);

  if(TEST_VERBOSE)
  {
    Debug_print("Interleaved packet\r\n");
    buffer_print_bin(packet_test_32x32, PACKET_LENGTH_32x32);
  }

  Packet_uninterleave_32x32(packet_test_32x32);

  if(TEST_VERBOSE)
  {
    Debug_print("Un-interleaved packet\r\n");
    buffer_print_bin(packet_test_32x32, PACKET_LENGTH_32x32);
  }

  if(memcmp(packet_test_32x32, packet_reference_32x32, PACKET_LENGTH_32x32) != 0)
  {
    return false;
  }


  /* Populate packet */
  for(i=0; i<PACKET_LENGTH_64x64; i++)
  {
    packet_reference_64x64[i] = (uint8_t)(Random(255));
    packet_test_64x64[i]      = packet_reference_64x64[i];
  }

  if(TEST_VERBOSE)
  {
    Debug_print("Original packet\r\n");
    buffer_print_bin(packet_test_64x64, PACKET_LENGTH_64x64);
  }

  /* Interleave packet */
  if(TEST_VERBOSE)
  {
    Debug_print("Interleaving packet..\r\n");
  }
  Packet_interleave_64x64(packet_test_64x64);

  if(TEST_VERBOSE)
  {
    Debug_print("Interleaved packet\r\n");
    buffer_print_bin(packet_test_64x64, PACKET_LENGTH_64x64);
  }

  Packet_uninterleave_64x64(packet_test_64x64);

  if(TEST_VERBOSE)
  {
    Debug_print("Un-interleaved packet\r\n");
    buffer_print_bin(packet_test_64x64, PACKET_LENGTH_64x64);
  }

  if(memcmp(packet_test_64x64, packet_reference_64x64, PACKET_LENGTH_64x64) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}