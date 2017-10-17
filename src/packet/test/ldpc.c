#include "../../firmware.h"
#include "../../test.h"

#define MESSAGE_LENGTH_TC512 32 // 256/8
#define CODEWORD_LENGTH_TC512 (2*MESSAGE_LENGTH_TC512)

uint8_t packet_reference_TC512[MESSAGE_LENGTH_TC512];

uint8_t packet_test_TC512[CODEWORD_LENGTH_TC512];

uint8_t decode_workbuf_TC512[LABRADOR_LDPC_BF_WORKING_LEN(TC512)];
uint8_t message_output_TC512[LABRADOR_LDPC_OUTPUT_LEN(TC512)];

static bool test_ldpc_TC512(void)
{
  uint32_t i;

  /* Populate packet */
  for(i=0; i<MESSAGE_LENGTH_TC512; i++)
  {
    packet_reference_TC512[i] = (uint8_t)(Random(255));
    packet_test_TC512[i]      = packet_reference_TC512[i];
  }

  if(TEST_VERBOSE)
  {
    Debug_print("Original message\r\n");
    buffer_print_bin(packet_test_TC512, MESSAGE_LENGTH_TC512);
  }

  /* Interleave packet */
  if(TEST_VERBOSE)
  {
    Debug_print("Encoding message..\r\n");
  }
  labrador_ldpc_encode(LABRADOR_LDPC_CODE_TC512, packet_test_TC512);

  if(TEST_VERBOSE)
  {
    Debug_print("Coded Message\r\n");
    buffer_print_bin(packet_test_TC512, CODEWORD_LENGTH_TC512);
  }

  /* Add random noise */
  #define BIT_FLIPS 10
  uint32_t bit_index;
  for(i=0; i<BIT_FLIPS; i++)
  {
    bit_index = (uint8_t)(Random(CODEWORD_LENGTH_TC512));
    packet_test_TC512[bit_index >> 3] = (uint8_t)(packet_test_TC512[bit_index >> 3] ^ (0x80 >> (bit_index & 0x07)));
  }

  size_t iters_count = 0;
  labrador_ldpc_decode_bf(LABRADOR_LDPC_CODE_TC512, packet_test_TC512, message_output_TC512, decode_workbuf_TC512, 50, &iters_count);

  if(TEST_VERBOSE)
  {
    Debug_print("Used %d/50 iterations\r\n", iters_count);
    Debug_print("Recovered message\r\n");
    buffer_print_bin(message_output_TC512, MESSAGE_LENGTH_TC512);
  }

  if(memcmp(message_output_TC512, packet_reference_TC512, MESSAGE_LENGTH_TC512) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool test_ldpc(void)
{
  return test_ldpc_TC512();
}