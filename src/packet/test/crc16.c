#include "../../firmware.h"
#include "../../test.h"

uint8_t pattern_0[50] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint32_t pattern_0_length = 50;
uint16_t pattern_0_test_output;
uint16_t pattern_0_expected_output = 0x570D;

uint8_t pattern_a[9] = "123456789";
uint32_t pattern_a_length = 9;
uint16_t pattern_a_test_output;
uint16_t pattern_a_expected_output = 0x29B1;

uint8_t pattern_b[40] = { 0x7F, 0x9C, 0x2B, 0xA4, 0xE8, 0x8F, 0x82, 0x7D,
                          0x61, 0x60, 0x45, 0x50, 0x76, 0x05, 0x85, 0x3E,
                          0x13, 0x1A, 0xB8, 0xD2, 0xB5, 0x94, 0x94, 0x6B,
                          0x9C, 0x81, 0x33, 0x3F, 0x9B, 0xB6, 0xE0, 0xCE,
                          0xF4, 0x3E, 0xA7, 0xA3, 0x36, 0x9D, 0x4B, 0xAA };
uint32_t pattern_b_length = 40;
uint16_t pattern_b_test_output;
uint16_t pattern_b_expected_output = 0x5264;

bool test_crc16(void)
{
  Packet_crc16(pattern_0, pattern_0_length, &pattern_0_test_output);

  if(TEST_VERBOSE)
  {
    Debug_print("CRC16 Output: 0x%04x\r\n", pattern_0_test_output);
  }

  if(pattern_0_test_output != pattern_0_expected_output)
  {
    return false;
  }

  Packet_crc16(pattern_a, pattern_a_length, &pattern_a_test_output);

  if(TEST_VERBOSE)
  {
    Debug_print("CRC16 Output: 0x%04x\r\n", pattern_a_test_output);
  }

  if(pattern_a_test_output != pattern_a_expected_output)
  {
    return false;
  }

  Packet_crc16(pattern_b, pattern_b_length, &pattern_b_test_output);

  if(TEST_VERBOSE)
  {
    Debug_print("CRC16 Output: 0x%04x\r\n", pattern_b_test_output);
  }

  if(pattern_b_test_output != pattern_b_expected_output)
  {
    return false;
  }

  return true;
}