/**
 * @file Crypto_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Private header file for crypto module
 * @version 0.1
 * @date 2020-12-18
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_CRYPTO_PRIVATE_H
#define H_CRYPTO_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdlib.h>

/* Internal includes */
#include "util/crypto/Crypto_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Width of a 32 bit CRC code.
 */
#define CRYPTO_CRC32_WIDTH (32)

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief Pre-computed CRC table for 32 bit CRCs.
 * 
 * This table is generated at compile time by Crypto_gen_crc_tables.py.
 */
extern const Crypto_Crc32 CRYPTO_CRC32_TABLE[256];

/**
 * @brief Pre-computed CRC table for 16 bit CRCs.
 * 
 * This table is generated at compile time by Crypto_gen_crc_tables.py.
 */
extern const Crypto_Crc16 CRYPTO_CRC16_TABLE[256];

#endif /* H_CRYPTO_PRIVATE_H */