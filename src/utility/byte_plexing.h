#ifndef __BYTE_PLEXING__
#define __BYTE_PLEXING__

#include <stdint.h>

void data_split_32(int32_t data, uint8_t *split);

void data_split_u32(uint32_t data, uint8_t *split);

void data_split_u16(uint16_t data, uint8_t *split);

void data_split_16(int16_t data, uint8_t *split);

uint32_t data_combine_u32(uint8_t *split_data);

uint32_t data_combine_u24(uint8_t *split_data);

#endif /* __BYTE_PLEXING__ */
