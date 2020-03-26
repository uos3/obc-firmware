/** @file test.h
 *
 * This file contains defines and helper macros for use in test files
 *
 * @author Phil Crump
 *
 * @addtogroup tests
 */

#ifndef __TEST_H__
#define __TEST_H__

#ifndef TEST_VERBOSE
  #define TEST_VERBOSE	false
#endif

#define assert_run(t, state)                                 \
  if(!(t))                                                      \
  {                                                             \
    state = false;                                              \
    Debug_print("\033[1;31m[ ABORT ]\033[0;00m\n");             \
  }

#define assert_run_print(t, s, state)                           \
  if((t))                                                       \
  {                                                             \
    Debug_print("\033[1;32m[ PASS ]\033[0;32m " s "\033[0;00m\n");   \
  }                                                             \
  else                                                          \
  {                                                             \
    Debug_print("\033[1;31m[ FAIL ]\033[0;31m " s "\033[0;00m\n");   \
    state = false;                                              \
  }

/**
 * @function
 * @ingroup tests
 *
 * Prints out out a buffer in decimal bytes
 *
 * @param buffer Input buffer pointer
 * @param length Length of input buffer in bytes
 */
void buffer_print_dec(uint8_t *buffer, uint32_t length);

/**
 * @function
 * @ingroup tests
 *
 * Prints out out a buffer in hexadecimal bytes
 *
 * @param buffer Input buffer pointer
 * @param length Length of input buffer in bytes
 */
void buffer_print_hex(uint8_t *buffer, uint32_t length);

/**
 * @function
 * @ingroup tests
 *
 * Prints out out a buffer in binary digits
 *
 * @param buffer Input buffer pointer
 * @param length Length of input buffer in bytes
 */
void buffer_print_bin(uint8_t *buffer, uint32_t length);

#include "packet/test/packet.h"
#include "buffer/test/buffer.h"
#include "utility/test/util.h"

#endif /*  __TEST_H__ */