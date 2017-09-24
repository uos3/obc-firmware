#include "../../firmware.h"
#include "../../test.h"

#define PACKET_LENGTH 128 // 1024/8

uint8_t packet_test[PACKET_LENGTH];
uint8_t packet_reference[PACKET_LENGTH];

bool test_interleave(void)
{
	uint32_t i;

  /* Populate packet */
  for(i=0; i<PACKET_LENGTH; i++)
  {
    packet_reference[i] = (uint8_t)(Random(255));
    packet_test[i]      = packet_reference[i];
  }

  if(TEST_VERBOSE)
  {
    Debug_print("Original packet\r\n");
    buffer_print_bin(packet_test, PACKET_LENGTH);
  }

  /* Interleave packet */
  if(TEST_VERBOSE)
  {
    Debug_print("Interleaving packet..\r\n");
  }
  packet_interleave_32x32(packet_test);

  if(TEST_VERBOSE)
  {
    Debug_print("Interleaved packet\r\n");
    buffer_print_bin(packet_test, PACKET_LENGTH);
  }

  packet_uninterleave_32x32(packet_test);

  if(TEST_VERBOSE)
  {
    Debug_print("Un-interleaved packet\r\n");
    buffer_print_bin(packet_test, PACKET_LENGTH);
  }

  if(memcmp(packet_test, packet_reference, PACKET_LENGTH) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}