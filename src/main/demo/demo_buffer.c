/**
 * File purpose:        FRAM buffer demo
 * Last modification:   16/09/2019
 * Status:              Developed using functions declared in the buffer/test folder made by Phil; Buffer functions are working correctly
 */

#include "../firmware.h"
#include <stdio.h>

#define TEST_VERBOSE 1
/* Declaration of test functions used - function copied and modified from buffer/test directory */
int test_buffer_add_remove_element(void);
int test_buffer_retrieve_element(void);
int test_buffer_roll(void);
int test_buffer_fill_elements(void);

int main(void){
    Board_init();
    watchdog_update = 0xFF;
    enable_watchdog_kick();
    UART_init(UART_INTERFACE, 9600);
    char output[200];

    for(int i=0;i<3;i++)
    {LED_on(LED_A); Delay_ms(100); LED_off(LED_A);} //blink the LED three times

    UART_puts(UART_INTERFACE, "\r\n>>>>>>>> FRAM Buffer Demo\r\n");
    
    int buffer_add_remove_element_result;
    int buffer_retrieve_element_result;
    int buffer_roll_result;
    int buffer_fill_elements_result;
    int occupied;
    while(1){
        buffer_add_remove_element_result = test_buffer_add_remove_element();
        sprintf(output, ">>> Test -> buffer_add_remove_element. Result -> %d\r\n", buffer_add_remove_element_result);
        UART_puts(UART_INTERFACE, output);
        Delay_ms(1000);

        buffer_retrieve_element_result = test_buffer_retrieve_element();
        sprintf(output, ">>> Test -> buffer_retrieve_element.   Result -> %d\r\n", buffer_retrieve_element_result);
        UART_puts(UART_INTERFACE, output);
        Delay_ms(1000);
        
        buffer_roll_result = test_buffer_roll();
        sprintf(output, ">>> Test -> buffer_roll.               Result -> %d\r\n", buffer_roll_result);
        UART_puts(UART_INTERFACE, output);
        Delay_ms(1000);

        buffer_fill_elements_result = test_buffer_fill_elements();
        sprintf(output, ">>> Test -> buffer_fill_elements.      Result -> %d\r\n", buffer_fill_elements_result);
        UART_puts(UART_INTERFACE, output);
        Delay_ms(1000);

        UART_puts(UART_INTERFACE, "\r\n");
        watchdog_update = 0xFF;
    }
}


int test_buffer_add_remove_element(void)
{
/* BUFFER_SLOT_SIZE is given in bits; one element uint8_t consists from 8 bits - so that's why division by 8*/
  uint8_t test_buffer_packet[BUFFER_SLOT_SIZE/8] = { 0 };   

  Buffer_init();
  /* Reset so we can predict data index = 0x01 */
  Buffer_reset();

  if(Buffer_count_occupied() != 0)
  {
    return 1;
  }

  Buffer_store_new_data(test_buffer_packet);
  if(Buffer_count_occupied() != 1)
  {
    return 2;
  }

  /* Remove incorrect index */
  Buffer_remove_index(0x34);
  if(Buffer_count_occupied() != 1)
  {
    return 3;
  }

  /* Remove correct index */
  Buffer_remove_index(0x01);
  if(Buffer_count_occupied() != 0)
  {
    return 4;
  }

  return 0;
}

int test_buffer_retrieve_element(void)
{
/* BUFFER_SLOT_SIZE is given in bits; one element uint8_t consists from 8 bits - so that's why division by 8*/
  uint32_t i;
  uint8_t reference_buffer_packet[BUFFER_SLOT_SIZE/8];
  uint8_t test_buffer_packet[BUFFER_SLOT_SIZE/8] = {0};

  /* Populate packet */
  for(i=0; i<BUFFER_SLOT_SIZE/8; i++)
  {
    reference_buffer_packet[i] = (uint8_t)(Random(255));
  }

  Buffer_init();
  Buffer_reset();

  /* Check slot #0 is empty */
  if(Buffer_get_occupancy(0x00))
  {
    return 1;
  }

  /* Attempt to retrieve a non-existent packet */
  if(Buffer_get_next_data(test_buffer_packet))
  {
    return 2;
  }

  /* Store a single packet */
  Buffer_store_new_data(reference_buffer_packet);

  /* Check slot #0 is occupied */
  if(!Buffer_get_occupancy(0x00))
  {
    return 3;
  }

  /* Retrieve the packet */
  if(!Buffer_get_next_data(test_buffer_packet))
  {
    return 4;
  }

  if(memcmp(test_buffer_packet, reference_buffer_packet, BUFFER_SLOT_SIZE/8) != 0)
  {
    return 5;
  }

  return 0;
}

int test_buffer_roll(void)
{
  uint32_t i;
  uint8_t reference1_buffer_packet[BUFFER_SLOT_SIZE/8];
  uint8_t reference2_buffer_packet[BUFFER_SLOT_SIZE/8];
  uint8_t test_buffer_packet[BUFFER_SLOT_SIZE/8] = {0};

  /* Populate packet */
  for(i=0; i<BUFFER_SLOT_SIZE/8; i++)
  {
    reference1_buffer_packet[i] = (uint8_t)(Random(255));
    reference2_buffer_packet[i] = (uint8_t)(Random(255));
  }

  Buffer_init();
  Buffer_reset();

  /* Check buffer is empty */
  if(Buffer_count_occupied() != 0)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Buffer count should be empty, is: %d\r\n", Buffer_count_occupied());
    }
    return 1;
  }

  /* Attempt to retrieve a non-existent packet */
  if(Buffer_get_next_data(test_buffer_packet))
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Succeeded in retrieving non-existent packet.\r\n");
    }
    return 2;
  }

  /* Store a single packet */
  Buffer_store_new_data(reference1_buffer_packet);

  /* Check buffer is empty */
  if(Buffer_count_occupied() != 1)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Buffer count should be 1, is: %d\r\n", Buffer_count_occupied());
    }
    return 3;
  }

  /* Retrieve the packet and verify it */
  if(!Buffer_get_next_data(test_buffer_packet))
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to retrieve packet (attempt 1).\r\n");
    }
    return 4;
  }
  if(memcmp(test_buffer_packet, reference1_buffer_packet, BUFFER_SLOT_SIZE/8) != 0)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to match retrieved packet (attempt 1).\r\n");
    }
    return 5;
  }

  /* Retrieve the packet and verify it */
  if(!Buffer_get_next_data(test_buffer_packet))
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to retrieve packet (attempt 2).\r\n");
    }
    return 6;
  }
  if(memcmp(test_buffer_packet, reference1_buffer_packet, BUFFER_SLOT_SIZE/8) != 0)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to match retrieved packet (attempt 2).\r\n");
    }
    return 7;
  }

  /* Check packet is still in the buffer */
  if(Buffer_count_occupied() != 1)
  {
    return 8;
  }

  /* Store next packet */
  Buffer_store_new_data(reference2_buffer_packet);

  /* Check new packet is in the buffer */
  if(Buffer_count_occupied() != 2)
  {
    return 9;
  }

  /* Retrieve the packet and verify it */
  if(!Buffer_get_next_data(test_buffer_packet))
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to retrieve packet2 (attempt 1).\r\n");
    }
    return 10;
  }
  if(memcmp(test_buffer_packet, reference2_buffer_packet, BUFFER_SLOT_SIZE/8) != 0)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to match retrieved packet2 (attempt 1).\r\n");
    }
    return 11;
  }

  /* Retrieve the packet and verify it */
  if(!Buffer_get_next_data(test_buffer_packet))
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to retrieve packet1 (attempt 3).\r\n");
    }
    return 12;
  }
  if(memcmp(test_buffer_packet, reference1_buffer_packet, BUFFER_SLOT_SIZE/8) != 0)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to match retrieved packet1 (attempt 3).\r\n");
    }
    return 13;
  }

  /* Retrieve the packet and verify it */
  if(!Buffer_get_next_data(test_buffer_packet))
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to retrieve packet2 (attempt 2).\r\n");
    }
    return 14;
  }
  if(memcmp(test_buffer_packet, reference2_buffer_packet, BUFFER_SLOT_SIZE/8) != 0)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to match retrieved packet2 (attempt 2).\r\n");
    }
    return 15;
  }

  /* Remove first packet - indexing starts with 1 */
  Buffer_remove_index(0x01);
  if(Buffer_count_occupied() != 1)
  {
    return 16;
  }

  /* Retrieve the packet and verify it */
  if(!Buffer_get_next_data(test_buffer_packet))
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to retrieve packet2 (attempt 3).\r\n");
    }
    return 17;
  }
  if(memcmp(test_buffer_packet, reference2_buffer_packet, BUFFER_SLOT_SIZE/8) != 0)
  {
    if(TEST_VERBOSE)
    {
      Debug_print("Failed to match retrieved packet2 (attempt 3).\r\n");
    }
    return 18;
  }

  return 0;
}

int test_buffer_fill_elements(void)
{
  uint8_t test_buffer_packet[BUFFER_SLOT_SIZE/8] = { 0 };

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
      return 1;
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
      return 2;
    }
  }

  return 0;
}
