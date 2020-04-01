#ifndef __BYTE_PLEXING__
#define __BYTE_PLEXING__

#include <stdint.h>

/*
	retreives the value of the byte of a variable

	args:
	var: the variable to read
	offset: the byte offset of the read operation

	returns:
	uint8_t var[offset], if var was already an array
*/
#define cast_asbyte(var, offset) *(((uint8_t*) &var)+offset)

/*
	casts the address of var as a uint8_t ptr

	args:
	var: the variable to cast
*/
#define cast_asptr(var) ((uint8_t*) &var)

// big endian means "big end first" (no, seriously, look it up)
#define big_endian (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
// little endian means "little end first"
#define little_endian (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

void data_split_32(int32_t data, uint8_t *split);

void data_split_u32(uint32_t data, uint8_t *split);

void data_split_u32_le(uint32_t data, uint8_t *split);

void data_split_u16(uint16_t data, uint8_t *split);

void data_split_16(int16_t data, uint8_t *split);

uint32_t data_combine_u32(uint8_t *split_data);

uint32_t data_combine_u24(uint8_t *split_data);

/*
	reverses the byte order of a list

	args:
	value_to_flip_ptr: the location of the variable to flip
	length: the length of the variable.
*/
void flip_endian(uint8_t* value_to_flip_ptr,  uint8_t length);

/*
	copies "value_size" bytes from contents of "variable_ptr" to "dest_array".
	the offset gives the byte-wise index to deposit the data. Increases the
	value of offset after the copy operation, ie
		offset = offset+value_addr
	and as such `offset` can be used to write the next value to the array:
		copy_to_array(array, &offset, &value1, 4, 32)
		copy_to_array(array, &offset, &value2, 4, 32)

	returns true on successful write, false if max size is exceeded.
*/
bool copy_to_array(void* array_start_addr, size_t array_max_length, int* offset, void* value_addr, size_t value_size);

#endif /* __BYTE_PLEXING__ */
