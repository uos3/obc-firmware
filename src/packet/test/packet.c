/* @file packet.c
 *
 * @author Phil Crump
 *
 * @ingroup tests
 * @{
 */

#include "../../firmware.h"
#include "../../test.h"

bool packet_tests(void)
{
  bool test_state = true;

  assert_run_print(test_interleave(), "Packet Interleave", 	test_state);
  assert_run_print(test_pn9(), 				"Packet PN9", 				test_state);
  assert_run_print(test_shake(), 			"Packet SHAKE-128", 	test_state);

  return test_state;
}

/**
 * @}
 */