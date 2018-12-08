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

#define BUFFER_SLOT_SIZE (uint32_t)848

#if(FRAM_SIZE == 0x20000)
 #define BUFFER_SLOTS 1210
#elif(FRAM_SIZE == 0x80000)
 #define BUFFER_SLOTS 4840
#else
  #error "FRAM Size not coded for."
#endif

#define BUFFER_FRAM_SIZE_LAST_INDEX     sizeof(uint16_t)
#define BUFFER_FRAM_SIZE_LAST_SLOT      sizeof(uint16_t)
#define BUFFER_FRAM_SIZE_OCCUPANCY      ROUND_UP(BUFFER_SLOTS/8, 1)
#define BUFFER_FRAM_SIZE_INDEXES        (BUFFER_SLOTS * sizeof(uint16_t))
#define BUFFER_FRAM_SIZE_CRC		        sizeof(uint16_t)

#define BUFFER_FRAM_ADDRESS_LAST_INDEX  0x000000
#define BUFFER_FRAM_ADDRESS_LAST_SLOT		(BUFFER_FRAM_ADDRESS_LAST_INDEX + BUFFER_FRAM_SIZE_LAST_INDEX)
#define BUFFER_FRAM_ADDRESS_OCCUPANCY   (BUFFER_FRAM_ADDRESS_LAST_SLOT + BUFFER_FRAM_SIZE_LAST_SLOT)
#define BUFFER_FRAM_ADDRESS_INDEXES     (BUFFER_FRAM_ADDRESS_OCCUPANCY + BUFFER_FRAM_SIZE_OCCUPANCY)
#define BUFFER_FRAM_ADDRESS_CRC	 	      (BUFFER_FRAM_ADDRESS_INDEXES + BUFFER_FRAM_SIZE_INDEXES)
#define BUFFER_FRAM_ADDRESS_SLOTS       (BUFFER_FRAM_ADDRESS_CRC + BUFFER_FRAM_SIZE_CRC)

/* Illustrative struct of FRAM data layout:
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
  uint16_t last_slot_transmitted;
  uint8_t occupancy[ROUND_UP(BUFFER_SLOTS/8, 1)]; // bitmap of occupancy
  uint16_t indexes[BUFFER_SLOTS]; // indexes[slot] = index
  uint16_t crc;
} buffer_cache_t;

void Buffer_init(void);
void Buffer_reset(void);
bool Buffer_verify_cache(void);

void Buffer_store_new_data(uint8_t *data_payload);
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

bool Buffer_FRAM_cache_read(buffer_cache_t *buffer);
void Buffer_FRAM_read_data(uint16_t slot, uint8_t *data);

void Buffer_FRAM_write_last_index_stored(buffer_cache_t *buffer);
void Buffer_FRAM_write_last_slot_transmitted(buffer_cache_t *buffer);
void Buffer_FRAM_write_occupancy(buffer_cache_t *buffer);
void Buffer_FRAM_write_indexes(buffer_cache_t *buffer);
void Buffer_FRAM_write_data(uint16_t slot, uint8_t *data);

#endif /* __BUFFER_H__ */