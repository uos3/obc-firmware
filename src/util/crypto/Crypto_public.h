/**
 * @file Crypto_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Simple cryptographic module.
 * 
 * This module implements standard cryptographic utilities, such as CRC
 * calculations.
 * 
 * Note: CRC is not strictly cryptographic but the general name is chosen to
 * include both crypto-like items and hash-like items.
 * 
 * @version 0.1
 * @date 2020-12-18
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_CRYPTO_PUBLIC_H
#define H_CRYPTO_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------   
 * TYPEDEFS
 * ------------------------------------------------------------------------- */

/**
 * @brief 32 bit CRC code.
 */
typedef uint32_t Crypto_Crc32;

/**
 * @brief 16 bit CRC code.
 */
typedef uint16_t Crypto_Crc16;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Calculate the 32 bit CRC for the given string, using the CRC-32C
 * algorithm.
 * 
 * This CRC uses reflected input and output.
 * 
 * @param p_data_in Pointer to the data to calculate the CRC for
 * @param length_in The length of the p_data_in pointer
 * @param p_crc_out Pointer to the CRC value to output.
 */
void Crypto_get_crc32(
    uint8_t *p_data_in, 
    size_t length_in, 
    Crypto_Crc32 *p_crc_out
);

/**
 * @brief Calculate the 16 bit CRC for the given string, using the CRC-32C
 * algorithm.
 * 
 * This CRC uses reflected input and output.
 * 
 * @param p_data_in Pointer to the data to calculate the CRC for
 * @param length_in The length of the p_data_in pointer
 * @param p_crc_out Pointer to the CRC value to output.
 */
void Crypto_get_crc16(
    uint8_t *p_data_in,
    size_t length_in,
    Crypto_Crc16 *p_crc_out
);

#endif /* H_CRYPTO_PUBLIC_H */