#include "../../firmware.h"
#include "../../test.h"

/* Test inputs */
const uint8_t crc_test_1[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const uint8_t crc_test_2[] = {
  0x7F, 0x9C, 0x2B, 0xA4, 0xE8, 0x8F, 0x82, 0x7D,
  0x61, 0x60, 0x45, 0x50, 0x76, 0x05, 0x85, 0x3E,
  0x13, 0x1A, 0xB8, 0xD2, 0xB5, 0x94, 0x94, 0x6B,
  0x9C, 0x81, 0x33, 0x3F, 0x9B, 0xB6, 0xE0, 0xCE,
  0xF4, 0x3E, 0xA7, 0xA3, 0x36, 0x9D, 0x4B, 0xAA
};
const uint8_t crc_test_3[] = {'1','2','3','4','5','6','7','8','9'};


// CRC-8

const uint8_t crc8_result_1 = 0x00;

const uint8_t crc8_result_2 = 0x18;

const uint8_t crc8_result_3 = 0xF4;

// CRC-16-CCITT-FALSE

const uint16_t crc16_result_1 = 0x570D;

const uint16_t crc16_result_2 = 0x5264;

const uint16_t crc16_result_3 = 0x29B1;

// CRC-32-CCITT

const uint32_t crc32_result_1 = 0x1F877C1E;

const uint32_t crc32_result_2 = 0xF62AE45F;

const uint32_t crc32_result_3 = 0xCBF43926;

bool test_crc(void)
{
  uint8_t crc8_result;
  uint16_t crc16_result;
  uint32_t crc32_result;

  /* CRC-8 Tests */

  crc8_result = Util_crc8(crc_test_1, sizeof(crc_test_1));
  if(crc8_result != crc8_result_1)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("CRC-8 Test 1: Expected 0x%02x, Got 0x%02x\r\n"
        , crc8_result_1
        , crc8_result
      );
    }
    return false;
  }

  crc8_result = Util_crc8(crc_test_2, sizeof(crc_test_2));
  if(crc8_result != crc8_result_2)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("CRC-8 Test 2: Expected 0x%02x, Got 0x%02x\r\n"
        , crc8_result_2
        , crc8_result
      );
    }
    return false;
  }

  crc8_result = Util_crc8(crc_test_3, sizeof(crc_test_3));
  if(crc8_result != crc8_result_3)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("CRC-8 Test 3: Expected 0x%02x, Got 0x%02x\r\n"
        , crc8_result_3
        , crc8_result
      );
    }
    return false;
  }

  /* CRC-16-CCITT-FALSE Tests */

  crc16_result = Util_crc16(crc_test_1, sizeof(crc_test_1));
  if(crc16_result != crc16_result_1)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("CRC-16-CCITT-FALSE Test 1: Expected 0x%04x, Got 0x%04x\r\n"
        , crc16_result_1
        , crc16_result
      );
    }
    return false;
  }

  crc16_result = Util_crc16(crc_test_2, sizeof(crc_test_2));
  if(crc16_result != crc16_result_2)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("CRC-16-CCITT-FALSE Test 2: Expected 0x%04x, Got 0x%04x\r\n"
        , crc16_result_2
        , crc16_result
      );
    }
    return false;
  }

  crc16_result = Util_crc16(crc_test_3, sizeof(crc_test_3));
  if(crc16_result != crc16_result_3)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("CRC-16-CCITT-FALSE Test 3: Expected 0x%04x, Got 0x%04x\r\n"
        , crc16_result_3
        , crc16_result
      );
    }
    return false;
  }

  /* CRC-32 Tests */

  crc32_result = Util_crc32(crc_test_1, sizeof(crc_test_1));
  if(crc32_result != crc32_result_1)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("CRC-32 Test 1: Expected 0x%08x, Got 0x%08x\r\n"
        , crc32_result_1
        , crc32_result
      );
    }
    return false;
  }

  crc32_result = Util_crc32(crc_test_2, sizeof(crc_test_2));
  if(crc32_result != crc32_result_2)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("CRC-32 Test 2: Expected 0x%08x, Got 0x%08x\r\n"
        , crc32_result_2
        , crc32_result
      );
    }
    return false;
  }

  crc32_result = Util_crc32(crc_test_3, sizeof(crc_test_3));
  if(crc32_result != crc32_result_3)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("CRC-32 Test 3: Expected 0x%08x, Got 0x%08x\r\n"
        , crc32_result_3
        , crc32_result
      );
    }
    return false;
  }

  return true;
}