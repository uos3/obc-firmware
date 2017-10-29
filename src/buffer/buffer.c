/**
 * @ingroup buffer
 *
 * @file buffer.c
 *
 * @{
 */

#include "../firmware.h"

/* Illustrative struct of data layout:
typedef struct buffer_data {
  uint16_t last_index;
  uint8_t occupancy[ROUND_UP(BUFFER_SLOTS/8, 1)];
  uint16_t indexes[BUFFER_SLOTS];
  buffer_data_slot slots[BUFFER_SLOTS];
} buffer_data;
*/

typedef struct buffer_cache_t {
  bool initialised;
  uint16_t last_index_stored;
  uint8_t occupancy[ROUND_UP(BUFFER_SLOTS/8, 1)];
  uint16_t indexes[BUFFER_SLOTS];
} buffer_cache_t;

static buffer_cache_t buffer_cache;

void Buffer_init(void)
{
  if(!buffer_cache.initialised)
  {
    FRAM_init();

    Buffer_FRAM_read_last_index_stored(&(buffer_cache.last_index_stored));
    Buffer_FRAM_read_occupancy(buffer_cache.occupancy);
    Buffer_FRAM_read_indexes(buffer_cache.indexes);

    buffer_cache.initialised = true;
  }
}

void Buffer_reset(void)
{
  buffer_cache.last_index_stored = 0;
  Buffer_FRAM_write_last_index_stored(&(buffer_cache.last_index_stored));

  memset(buffer_cache.occupancy, 0x00, BUFFER_FRAM_SIZE_OCCUPANCY);
  Buffer_FRAM_write_occupancy(buffer_cache.occupancy);

  memset(buffer_cache.indexes, 0x00, BUFFER_FRAM_SIZE_INDEXES);
  Buffer_FRAM_write_indexes(buffer_cache.indexes);

  buffer_cache.initialised = false;
  Buffer_init();
}

void Buffer_store_new_data(uint8_t *data_payload)
{
  uint16_t slot;

  Buffer_find_new_slot(&slot);
  Buffer_FRAM_write_data(slot, data_payload);
  Buffer_set_occupancy(slot, true);
}

bool Buffer_get_next_data(uint16_t *previous_slot, uint8_t *data_payload)
{
  if(Buffer_get_next_slot(previous_slot))
  {
    Buffer_FRAM_read_data(*previous_slot, data_payload);
    return true;
  }
  return false;
}

bool Buffer_get_next_slot(uint16_t *slot)
{
  uint16_t new_slot = *slot;
  /* Return next occupied slot */
  while(0x00 == (buffer_cache.occupancy[new_slot>>3] & (0x80 >> (new_slot & 0x07))))
  {
    if(++new_slot >= BUFFER_SLOTS)
    {
      /* Wrap search */
      new_slot = 0;
    }
    else if(new_slot == *slot)
    {
      /* Reached beginning, no occupied slots */
      return false;
    }
  }
  *slot = new_slot;
  return true;
}

void Buffer_remove_index(uint16_t index)
{
  uint16_t slot;
  if(Buffer_find_index(index, &slot))
  {
    Buffer_set_occupancy(slot, false);
  }
}

bool Buffer_find_index(uint16_t index, uint16_t *slot)
{
  *slot = 0;
  while(buffer_cache.indexes[*slot] != index)
  {
    if(++*slot >= BUFFER_SLOTS)
    {
      return false;
    }
  }
  return true;
}

void Buffer_find_new_slot(uint16_t *slot)
{
  *slot = 0;
  while (buffer_cache.occupancy[*slot>>3] & (0x80 >> (*slot & 0x07)))
  {
    if(++*slot >= BUFFER_SLOTS)
    {
      /* No unoccupied slots, so find slot with oldest index */
      Buffer_find_oldest_slot(slot);
      return;
    }
  }
}

void Buffer_find_oldest_slot(uint16_t *slot)
{
  uint16_t oldest_slot, oldest_index = buffer_cache.last_index_stored;

  *slot = 0;
  while(++*slot < BUFFER_SLOTS)
  {
    if(oldest_index <= buffer_cache.last_index_stored)
    {
      if(buffer_cache.indexes[*slot] < oldest_index)
      {
        oldest_index = buffer_cache.indexes[*slot];
        oldest_slot = *slot;
      }
      else if(buffer_cache.indexes[*slot] > buffer_cache.last_index_stored)
      {
        oldest_index = buffer_cache.indexes[*slot];
        oldest_slot = *slot;
      }
    }
    else if(buffer_cache.indexes[*slot] > buffer_cache.last_index_stored)
    {
      if(buffer_cache.indexes[*slot] < oldest_index)
      {
        oldest_index = buffer_cache.indexes[*slot];
        oldest_slot = *slot;
      }
    }
  }

  *slot = oldest_slot;
  return;
}

bool Buffer_get_occupancy(uint16_t slot)
{
  return buffer_cache.occupancy[slot>>3] & (0x80 >> (slot & 0x07));
}

void Buffer_set_occupancy(uint16_t slot, bool value)
{
  if(value)
  {
    /* Set the corresponding bit */
    buffer_cache.occupancy[slot>>3] = (uint8_t)(buffer_cache.occupancy[slot>>3] | (0x80 >> (slot & 0x07)));
  }
  else
  {
    /* Reset the corresponding bit */
    buffer_cache.occupancy[slot>>3] = (uint8_t)(buffer_cache.occupancy[slot>>3] &  ~(0x80 >> (slot & 0x07)));
  }
  Buffer_FRAM_write_occupancy(buffer_cache.occupancy);
}

uint16_t Buffer_count_occupied(void)
{
  uint16_t i, result = 0;
  for(i=0; i<BUFFER_SLOTS; i++)
  {
    if(buffer_cache.occupancy[i>>3] & (0x80 >> (i & 0x07)))
    {
      result++;
    }
  }
  return result;
}

/**
 * @}
 */