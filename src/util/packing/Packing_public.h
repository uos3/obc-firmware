/**
 * @file Packing_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Utility module to assist in packing of bytes to and from byte streams.
 * 
 * @version 0.1
 * @date 2021-04-21
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_PACKING_PUBLIC_H
#define H_PACKING_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Pack a pair of big endian bytes into the machine-specific uint16 
 * type. 
 * 
 * @param p_data_in Pointer to the data to pack, shall be at least 2 bytes in 
 * length. 
 * @return uint16_t The value of the packed bytes.
 */
uint16_t Packing_u16_from_be(uint8_t *p_data_in);

/**
 * @brief Pack a pair of little endian bytes into the machine-specific uint16 
 * type. 
 * 
 * @param p_data_in Pointer to the data to pack, shall be at least 2 bytes in 
 * length. 
 * @return uint16_t The value of the packed bytes.
 */
uint16_t Packing_u16_from_le(uint8_t *p_data_in);

/**
 * @brief Pack a machine-specific uint16_t into a big endian byte array.
 * 
 * @param data_in The uint16_t value to pack.
 * @param p_data_out Pointer to the bytes to populate. Shall be at least 2
 * bytes in length.
 */
void Packing_u16_to_be(uint16_t data_in, uint8_t *p_data_out);

/**
 * @brief Pack a machine-specific uint16_t into a little endian byte array.
 * 
 * @param data_in The uint16_t value to pack.
 * @param p_data_out Pointer to the bytes to populate. Shall be at least 2
 * bytes in length.
 */
void Packing_u16_to_le(uint16_t data_in, uint8_t *p_data_out);

#endif /* H_PACKING_PUBLIC_H */