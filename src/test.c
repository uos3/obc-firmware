/** @file test.c
 *
 * This file contains the main function for the module tests.
 *
 * @author Phil Crump
 *
 * @ingroup tests
 * @{
 */

#include "firmware.h"
#include "test.h"

/**
 * @brief Runs all module tests
 *
 * @returns 0 if all tests were successful, -1 otherwise.
 */
int main(void)
{
  bool test_state = true;
  Debug_print("## UoS³ Module Tests ##\n");

  /* Call module tests */
  assert_run(packet_tests(), test_state);

  if(test_state)
    return 0;
  else
    return -1;
}

/**
 * @brief Prints out out a buffer in decimal bytes
 *
 * @param buffer Input buffer pointer
 * @param length Length of input buffer in bytes
 */
void buffer_print_dec(uint8_t *buffer, uint32_t length)
{
  uint32_t i = 0;
  uint8_t row = 20;
  while(i<length)
  {
    if(row == 0)
    {
      Debug_print("\r\n");
      row = 20;
    }
    Debug_print("%4d", buffer[i]);
    row--;
    i++;
  }
  Debug_print("\r\n");
}

/**
 * @brief Prints out out a buffer in hexadecimal bytes
 *
 * @param buffer Input buffer pointer
 * @param length Length of input buffer in bytes
 */
void buffer_print_hex(uint8_t *buffer, uint32_t length)
{
  uint32_t i = 0;
  uint8_t row = 20;
  while(i<length)
  {
    if(row == 0)
    {
      Debug_print("\r\n");
      row = 20;
    }
    Debug_print(" %02x", buffer[i]);
    row--;
    i++;
  }
  Debug_print("\r\n");
}

/**
 * @brief Prints out out a buffer in binary digits
 *
 * @param buffer Input buffer pointer
 * @param length Length of input buffer in bytes
 */
void buffer_print_bin(uint8_t *buffer, uint32_t length)
{
  uint32_t i = 0, j, m;
  uint8_t row = 4;
  while(i<length)
  {
    if(row == 0)
    {
      Debug_print("\r\n");
      row = 4;
    }
    j=7;
    m=0x80;
    while(m!=0)
   	{
   		Debug_print(" %01d", (buffer[i] & m) >> j);
   		m >>= 1;
   		j--;
   	}
    row--;
    i++;
  }
  Debug_print("\r\n");
}

/**
 * @}
 */