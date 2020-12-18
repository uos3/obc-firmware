/**
 * @file Crypto_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Implementation of the crypto module. See corresponding header for
 * more information.
 * 
 * @version 0.1
 * @date 2020-12-18
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdlib.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "util/crypto/Crypto_public.h"
#include "util/crypto/Crypto_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void Crypto_get_crc32(
    uint8_t *p_data_in, 
    size_t length_in, 
    Crypto_Crc32 *p_crc_out
) {
    /* 
     * See
     * https://reveng.sourceforge.io/crc-catalogue/17plus.htm#crc.cat.crc-32c 
     */

    uint8_t table_idx;
    uint32_t reflected;
    *p_crc_out = 0xffffffff;
    
    /* Divide input data by the polynomial, byte at a time */
    for (int32_t i = 0; i < length_in; ++i) {
        table_idx = (*p_crc_out ^ p_data_in[i]) & 0xFF;
        *p_crc_out = CRYPTO_CRC32_TABLE[table_idx] ^ (*p_crc_out >> 8);
    }

    /* XOR the final CRC with the value given in the CRC catalogue*/
    *p_crc_out = *p_crc_out ^ 0xFFFFFFFF;
}