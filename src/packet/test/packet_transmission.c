#include "../../firmware.h"
#include "../../test.h"

bool test_packet_transmission_telecommand(void)
{
  uint32_t i;
  uint16_t spacecraft_id = 0x4242;
  uint8_t tc_sign_key[16] = { 0x45, 0xDA, 0x45, 0x92, 0xF7, 0x44, 0xD4, 0xEB,
                                    0x6C, 0x81, 0x34, 0x3F, 0x9C, 0xB6, 0xE0, 0xCE };

  packet_telecommand_512 reference_packet;
  packet_telecommand_512 test_tx_packet;
  packet_telecommand_512 test_rx_packet;

  uint8_t transmission_buffer[128]; // 1024b

  /* Construct sample packet data */
  reference_packet.spacecraft = spacecraft_id;
  for(i=0; i<sizeof(reference_packet.data); i++)
  {
    reference_packet.data[i] = (uint8_t)(Random(255));
  }

  if(TEST_VERBOSE)
  {
    Debug_print("Original message\r\n");
    buffer_print_hex((uint8_t *)&reference_packet, sizeof(reference_packet));
  }

  memcpy(&test_tx_packet, &reference_packet, sizeof(test_tx_packet));

  Packet_telecommand_512_encode(&test_tx_packet, transmission_buffer, spacecraft_id, tc_sign_key, sizeof(tc_sign_key));

  if(TEST_VERBOSE)
  {
    Debug_print("Coded Message\r\n");
    buffer_print_hex(transmission_buffer, sizeof(transmission_buffer));
  }

  /* Add random noise (simulate transmission channel) */
  #define BIT_FLIPS 10
  uint32_t bit_index;
  for(i=0; i<BIT_FLIPS; i++)
  {
    bit_index = (uint8_t)(Random(sizeof(transmission_buffer)));
    transmission_buffer[bit_index >> 3] = (uint8_t)(transmission_buffer[bit_index >> 3] ^ (0x80 >> (bit_index & 0x07)));
  }

  /* Attempt decode */
  if(!Packet_telecommand_512_decode(transmission_buffer, &test_rx_packet, spacecraft_id, tc_sign_key, sizeof(tc_sign_key)))
  {
    if(1)
    {
      Debug_print("Decode Failed\r\n");
      Debug_print("reference\r\n");
      buffer_print_hex((uint8_t *)&reference_packet, sizeof(reference_packet));
      Debug_print("transmitted\r\n");
      buffer_print_hex((uint8_t *)&test_rx_packet, sizeof(test_rx_packet));
    }
    return false;
  }

  /* Compare data with ref packet */
  if(memcmp(test_rx_packet.data, reference_packet.data, sizeof(reference_packet.data)) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool test_packet_transmission_telemetry(void)
{
  
  uint32_t i;
  uint16_t spacecraft_id = 0x4242;
  uint8_t tm_sign_key[16] = { 0x45, 0xDA, 0x82, 0xBB, 0xF7, 0x44, 0xD4, 0x98,
                                    0x6C, 0xA3, 0x33, 0x3E, 0x9B, 0xB6, 0xEB, 0xCE };

  packet_telemetry_1024 reference_packet;
  packet_telemetry_1024 test_tx_packet;
  packet_telemetry_1024 test_rx_packet;

  uint8_t transmission_buffer[384]; // 3072b

  /* Construct sample packet data */
  reference_packet.spacecraft = spacecraft_id;
  for(i=0; i<sizeof(reference_packet.data); i++)
  {
    reference_packet.data[i] = (uint8_t)(Random(255));
  }

  if(TEST_VERBOSE)
  {
    Debug_print("Original message\r\n");
    buffer_print_hex((uint8_t *)&reference_packet, sizeof(reference_packet));
  }

  memcpy(&test_tx_packet, &reference_packet, sizeof(test_tx_packet));

  Packet_telemetry_1024_encode(&test_tx_packet, transmission_buffer, spacecraft_id, tm_sign_key, sizeof(tm_sign_key));

  if(TEST_VERBOSE)
  {
    Debug_print("Coded Message\r\n");
    buffer_print_hex(transmission_buffer, sizeof(transmission_buffer));
  }

  /* Add random noise (simulate transmission channel) */
  /* TODO: Add back in when we have FEC
  #define BIT_FLIPS 10
  uint32_t bit_index;
  for(i=0; i<BIT_FLIPS; i++)
  {
    bit_index = (uint8_t)(Random(sizeof(transmission_buffer)));
    transmission_buffer[bit_index >> 3] = (uint8_t)(transmission_buffer[bit_index >> 3] ^ (0x80 >> (bit_index & 0x07)));
  }
  */

  /* Attempt decode */
  if(!Packet_telemetry_1024_decode(transmission_buffer, &test_rx_packet, spacecraft_id, tm_sign_key, sizeof(tm_sign_key)))
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Decode Failed\r\n");
      Debug_print("reference\r\n");
      buffer_print_hex((uint8_t *)&reference_packet, sizeof(reference_packet));
      Debug_print("transmitted\r\n");
      buffer_print_hex((uint8_t *)&test_rx_packet, sizeof(test_rx_packet));
    }
    return false;
  }

  /* Compare data with ref packet */
  if(memcmp(test_rx_packet.data, reference_packet.data, sizeof(reference_packet.data)) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}