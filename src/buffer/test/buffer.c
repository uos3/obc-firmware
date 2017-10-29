/* @file buffer.c
 *
 * @author Phil Crump
 *
 * @ingroup tests
 * @{
 */

#include "../../firmware.h"
#include "../../test.h"

bool test_buffer_add_element(void);

bool buffer_tests(void)
{
  bool test_state = true;

  Buffer_init();
  Buffer_reset();

  assert_run_print(test_buffer_add_element(), "Buffer Add Element", 	test_state);

  return test_state;
}

uint8_t test_buffer_packet[848] = { 0 };

bool test_buffer_add_element(void)
{
	uint16_t count = Buffer_count_occupied();

	Buffer_store_new_data(test_buffer_packet);

	if(Buffer_count_occupied() != (count + 1))
	{
		return false;
	}

	return true;
}

/**
 * @}
 */