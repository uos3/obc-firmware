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

  assert_run_print(test_interleave(), 	"Packet Interleave", 			test_state);
  assert_run_print(test_sign(), 		"Packet SHAKE-128 Signing", 	test_state);
  assert_run_print(test_ldpc(), 		"Packet LDPC", 	      			test_state);

  return test_state;
}

/**
 * @}
 */