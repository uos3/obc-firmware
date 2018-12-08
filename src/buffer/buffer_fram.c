/**
 * @ingroup buffer
 *
 * @file buffer_fram.c
 *
 * @{
 */

#include "../firmware.h"

/*** Read ***/

bool Buffer_FRAM_cache_read(buffer_cache_t *buffer)
{
  FRAM_read(0x00000000, (uint8_t *)buffer, sizeof(buffer_cache_t));

  if(buffer->crc == Util_crc16((uint8_t *)&buffer, sizeof(buffer_cache_t) - sizeof(uint16_t)))
  {
    return true;
  }
  return false;
}

void Buffer_FRAM_read_data(uint16_t slot, uint8_t *data)
{
  FRAM_read((uint32_t)(BUFFER_FRAM_ADDRESS_SLOTS + (slot * BUFFER_SLOT_SIZE)), data, BUFFER_SLOT_SIZE);
}

/*** Write ***/

static inline void Buffer_FRAM_update_crc(buffer_cache_t *buffer)
{
  buffer->crc = Util_crc16((uint8_t *)&buffer, sizeof(buffer_cache_t) - sizeof(uint16_t));
  FRAM_write(BUFFER_FRAM_ADDRESS_CRC, (uint8_t *)&(buffer->crc), BUFFER_FRAM_SIZE_CRC);
}

void Buffer_FRAM_write_last_index_stored(buffer_cache_t *buffer)
{
  FRAM_write(BUFFER_FRAM_ADDRESS_LAST_INDEX, (uint8_t *)&(buffer->last_index_stored), sizeof(buffer->last_index_stored));
  Buffer_FRAM_update_crc(buffer);
}

void Buffer_FRAM_write_last_slot_transmitted(buffer_cache_t *buffer)
{
  FRAM_write(BUFFER_FRAM_ADDRESS_LAST_SLOT, (uint8_t *)&(buffer->last_slot_transmitted), sizeof(buffer->last_slot_transmitted));
  Buffer_FRAM_update_crc(buffer);
}

void Buffer_FRAM_write_occupancy(buffer_cache_t *buffer)
{
  FRAM_write(BUFFER_FRAM_ADDRESS_OCCUPANCY, (uint8_t *)&(buffer->occupancy), sizeof(buffer->occupancy));
  Buffer_FRAM_update_crc(buffer);
}

void Buffer_FRAM_write_indexes(buffer_cache_t *buffer)
{
  FRAM_write(BUFFER_FRAM_ADDRESS_INDEXES, (uint8_t *)&(buffer->indexes), sizeof(buffer->indexes));
  Buffer_FRAM_update_crc(buffer);
}

void Buffer_FRAM_write_data(uint16_t slot, uint8_t *data)
{
  FRAM_write((uint32_t)(BUFFER_FRAM_ADDRESS_SLOTS + (slot * BUFFER_SLOT_SIZE)), data, BUFFER_SLOT_SIZE);
}

/**
 * @}
 */