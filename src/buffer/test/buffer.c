/* @file buffer.c
 *
 * @author Phil Crump
 *
 * @ingroup tests
 * @{
 */

#include "../../firmware.h"
#include "../../test.h"

bool test_buffer_add_remove_element(void);
bool test_buffer_retrieve_element(void);
bool test_buffer_fill_elements(void);

bool buffer_tests(void)
{
  bool test_state = true;

  assert_run_print(test_buffer_add_remove_element(), "Buffer Add & Remove Element", 	test_state);

  assert_run_print(test_buffer_retrieve_element(), "Buffer Retrieve Element", 	test_state);

  assert_run_print(test_buffer_fill_elements(), "Buffer Fill & Overflow", 	test_state);

  return test_state;
}


bool test_buffer_add_remove_element(void)
{
	uint8_t test_buffer_packet[BUFFER_SLOT_SIZE] = { 0 };

  Buffer_init();
  /* Reset so we can predict data index = 0x01 */
  Buffer_reset();

	if(Buffer_count_occupied() != 0)
	{
		return false;
	}

	Buffer_store_new_data(test_buffer_packet);
	if(Buffer_count_occupied() != 1)
	{
		return false;
	}

	/* Remove incorrect index */
	Buffer_remove_index(0x34);
	if(Buffer_count_occupied() != 1)
	{
		return false;
	}

	/* Remove correct index */
	Buffer_remove_index(0x01);
	if(Buffer_count_occupied() != 0)
	{
		return false;
	}

	return true;
}

bool test_buffer_retrieve_element(void)
{
	uint32_t i;
	uint8_t reference_buffer_packet[BUFFER_SLOT_SIZE];
	uint8_t test_buffer_packet[BUFFER_SLOT_SIZE] = {0};

	/* Populate packet */
  for(i=0; i<BUFFER_SLOT_SIZE; i++)
  {
    reference_buffer_packet[i] = (uint8_t)(Random(255));
  }

  Buffer_init();
  Buffer_reset();

	Buffer_store_new_data(reference_buffer_packet);

	if(!Buffer_get_next_data(test_buffer_packet))
	{
		return false;
	}

	if(memcmp(test_buffer_packet, reference_buffer_packet, BUFFER_SLOT_SIZE) != 0)
	{
		return false;
	}

	return true;
}

bool test_buffer_fill_elements(void)
{
	uint8_t test_buffer_packet[BUFFER_SLOT_SIZE] = { 0 };

  Buffer_init();
  Buffer_reset();

  /* Fill buffer */
	for(uint16_t i=0; i<BUFFER_SLOTS; i++)
	{
		Buffer_store_new_data(test_buffer_packet);

		if(Buffer_count_occupied() != i+1)
		{
			if(TEST_VERBOSE)
		  {
		    Debug_print("i: %d Occupied: %d\r\n", i, Buffer_count_occupied());
		  }
			return false;
		}
	}

	/* Check it holds capacity */
	for(uint16_t i=0; i<BUFFER_SLOTS; i++)
	{
		Buffer_store_new_data(test_buffer_packet);

		if(Buffer_count_occupied() != BUFFER_SLOTS)
		{
			if(TEST_VERBOSE)
		  {
		    Debug_print("i: %d Occupied: %d\r\n", i, Buffer_count_occupied());
		  }
			return false;
		}
	}

	return true;
}

/**
 * @}
 */