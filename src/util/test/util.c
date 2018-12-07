/* @file util.c
 *
 * @author Phil Crump
 *
 * @ingroup tests
 * @{
 */

#include "../../firmware.h"
#include "../../test.h"

bool util_tests(void)
{
  bool test_state = true;

  assert_run_print(test_crc(), "Util CRC", 	test_state);
  assert_run_print(test_pn9(), "Util PN9", 	test_state);

  return test_state;
}

/**
 * @}
 */