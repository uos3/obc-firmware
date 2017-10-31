/**
 * @defgroup buffer
 *
 * @brief Buffer Functions & Definitions
 *
 * @details 
 *
 * @ingroup buffer
 */

#ifndef __BUFFER_H__
#define __BUFFER_H__

#define BUFFER_SLOT_SIZE 848

#if(FRAM_SIZE == 0x20000)
 #define BUFFER_SLOTS 1210
#elif(FRAM_SIZE == 0x80000)
 #define BUFFER_SLOTS 4840
#else
  #error "FRAM Size not coded for."
#endif

#define BUFFER_FRAM_SIZE_LAST_INDEX     2
#define BUFFER_FRAM_SIZE_LAST_SLOT      2
#define BUFFER_FRAM_SIZE_OCCUPANCY      ROUND_UP(BUFFER_SLOTS/8, 1)
#define BUFFER_FRAM_SIZE_INDEXES        (BUFFER_SLOTS * 2)

#define BUFFER_FRAM_ADDRESS_LAST_INDEX  0x000000
#define BUFFER_FRAM_ADDRESS_LAST_SLOT		(BUFFER_FRAM_ADDRESS_LAST_INDEX + BUFFER_FRAM_SIZE_LAST_INDEX)
#define BUFFER_FRAM_ADDRESS_OCCUPANCY   (BUFFER_FRAM_ADDRESS_LAST_SLOT + BUFFER_FRAM_SIZE_LAST_SLOT)
#define BUFFER_FRAM_ADDRESS_INDEXES     (BUFFER_FRAM_ADDRESS_OCCUPANCY + BUFFER_FRAM_SIZE_OCCUPANCY)
#define BUFFER_FRAM_ADDRESS_SLOTS       (BUFFER_FRAM_ADDRESS_INDEXES + BUFFER_FRAM_SIZE_INDEXES)

void Buffer_init(void);
void Buffer_reset(void);

void Buffer_store_new_data(uint16_t index, uint8_t *data_payload);
bool Buffer_get_next_data(uint8_t *data_payload);
void Buffer_remove_index(uint16_t index);
uint16_t Buffer_count_occupied(void);


bool Buffer_get_next_slot(uint16_t *slot);
bool Buffer_find_index(uint16_t index, uint16_t *slot);
void Buffer_find_new_slot(uint16_t *slot);
void Buffer_find_oldest_slot(uint16_t *slot);
bool Buffer_get_occupancy(uint16_t slot);
void Buffer_set_occupancy(uint16_t slot, bool value);
void Buffer_set_index(uint16_t slot, uint16_t index);


void Buffer_FRAM_write_last_index_stored(uint16_t *index);
void Buffer_FRAM_read_last_index_stored(uint16_t *index);

void Buffer_FRAM_write_last_slot_transmitted(uint16_t *slot);
void Buffer_FRAM_read_last_slot_transmitted(uint16_t *slot);

void Buffer_FRAM_write_occupancy(uint8_t *occupancy);
void Buffer_FRAM_read_occupancy(uint8_t *occupancy);

void Buffer_FRAM_write_indexes(uint16_t *indexes);
void Buffer_FRAM_read_indexes(uint16_t *indexes);

void Buffer_FRAM_write_data(uint16_t slot, uint8_t *data);
void Buffer_FRAM_read_data(uint16_t slot, uint8_t *data);

#endif /* __BUFFER_H__ */