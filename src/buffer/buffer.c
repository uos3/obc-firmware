/**
 * @ingroup buffer
 *
 * @file buffer.c
 *
 * @{
 */

#include "../firmware.h"

static buffer_cache_t buffer_cache = { false, 0, 0, {0}, {0}, 0x0000 };

void Buffer_init(void)
{
  if(buffer_cache.initialised && !Buffer_verify_cache())
  {
    /* TODO: SEU Detected, REBOOT! */
    Mission_SEU();
  }
  else if(!buffer_cache.initialised)
  {
    if(!Buffer_FRAM_cache_read(&buffer_cache))
    {
      /* Stored FRAM header fails CRC, so reset the buffer */
      /* TODO: Increment a Counter in health data */
      Buffer_reset();
    }

    buffer_cache.initialised = true;
  }
}

bool Buffer_verify_cache(void)
{
  return (buffer_cache.crc == Util_crc16((uint8_t *)&buffer_cache, sizeof(buffer_cache_t) - sizeof(uint16_t)));
}

void Buffer_reset(void)
{
  buffer_cache.last_index_stored = 0;
  Buffer_FRAM_write_last_index_stored(&buffer_cache);

  buffer_cache.last_slot_transmitted = (BUFFER_SLOTS-1);
  Buffer_FRAM_write_last_slot_transmitted(&buffer_cache);

  memset(buffer_cache.occupancy, 0x00, BUFFER_FRAM_SIZE_OCCUPANCY);
  Buffer_FRAM_write_occupancy(&buffer_cache);

  memset(buffer_cache.indexes, 0x00, BUFFER_FRAM_SIZE_INDEXES);
  Buffer_FRAM_write_indexes(&buffer_cache);

  /* CRC is automatically updated on each FRAM write */
}

void Buffer_store_new_data(uint8_t *data_payload)
{
  Buffer_init();

  uint16_t new_slot;
  uint16_t new_index;

  new_index = (uint16_t)(buffer_cache.last_index_stored + 1);

  Buffer_find_new_slot(&new_slot);
  Buffer_FRAM_write_data(new_slot, data_payload);
  Buffer_set_index(new_slot, new_index);
  Buffer_set_occupancy(new_slot, true);
}

void Buffer_set_index(uint16_t slot, uint16_t index)
{
  buffer_cache.indexes[slot] = index;
  Buffer_FRAM_write_indexes(&buffer_cache);
}

bool Buffer_get_next_data(uint8_t *data_payload)
{
  Buffer_init();
  
  if(Buffer_get_next_slot(&(buffer_cache.last_slot_transmitted)))
  {
    Buffer_FRAM_read_data(buffer_cache.last_slot_transmitted, data_payload);
    return true;
  }
  return false;
}

bool Buffer_get_next_slot(uint16_t *slot)
{
  uint16_t new_slot = *slot;

  if(++new_slot >= BUFFER_SLOTS)
  {
    /* Wrap search */
    new_slot = 0;
  }

  while(new_slot != *slot)
  {
    /* Check if slot is occupied */
    if(0x00 != (buffer_cache.occupancy[new_slot>>3] & (0x80 >> (new_slot & 0x07))))
    {
      *slot = new_slot;
      return true;
    }
    //printf("%d\n", new_slot);

    if(++new_slot >= BUFFER_SLOTS)
    {
      /* Wrap search */
      new_slot = 0;
    }
  }

  return false;
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
  Buffer_FRAM_write_occupancy(&buffer_cache);
}

uint16_t Buffer_count_occupied(void)
{
  Buffer_init();
  
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