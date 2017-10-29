/**
 * @ingroup buffer
 *
 * @file buffer_fram.c
 *
 * @{
 */

#include "../firmware.h"

void Buffer_FRAM_write_last_index_stored(uint16_t *index)
{
  FRAM_read(BUFFER_FRAM_ADDRESS_LAST_INDEX, (uint8_t*)index, BUFFER_FRAM_SIZE_LAST_INDEX);
}

void Buffer_FRAM_read_last_index_stored(uint16_t *index)
{
  FRAM_write(BUFFER_FRAM_ADDRESS_LAST_INDEX, (uint8_t*)index, BUFFER_FRAM_SIZE_LAST_INDEX);
}


void Buffer_FRAM_write_occupancy(uint8_t *occupancy)
{
  FRAM_write(BUFFER_FRAM_ADDRESS_OCCUPANCY, occupancy, BUFFER_FRAM_SIZE_OCCUPANCY);
}

void Buffer_FRAM_read_occupancy(uint8_t *occupancy)
{
  FRAM_read(BUFFER_FRAM_ADDRESS_OCCUPANCY, occupancy, BUFFER_FRAM_SIZE_OCCUPANCY);
}


void Buffer_FRAM_write_indexes(uint16_t *indexes)
{
  FRAM_write(BUFFER_FRAM_ADDRESS_INDEXES, (uint8_t*)indexes, BUFFER_FRAM_SIZE_INDEXES);
}

void Buffer_FRAM_read_indexes(uint16_t *indexes)
{
  FRAM_read(BUFFER_FRAM_ADDRESS_INDEXES, (uint8_t*)indexes, BUFFER_FRAM_SIZE_INDEXES);
}


void Buffer_FRAM_write_data(uint16_t slot, uint8_t *data)
{
  FRAM_write((uint32_t)(BUFFER_FRAM_ADDRESS_SLOTS + (slot * BUFFER_SLOT_SIZE)), data, BUFFER_SLOT_SIZE);
}

void Buffer_FRAM_read_data(uint16_t slot, uint8_t *data)
{
  FRAM_read((uint32_t)(BUFFER_FRAM_ADDRESS_SLOTS + (slot * BUFFER_SLOT_SIZE)), data, BUFFER_SLOT_SIZE);
}

/**
 * @}
 */