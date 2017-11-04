/**
 * @defgroup shake
 *
 * @brief Packet SHAKE Hash Routines
 *
 * @details 
 *
 * Copyright (c) 2015 Markku-Juhani O. Saarinen
 * The MIT License (MIT)
 *
 * @ingroup packet
 */

#ifndef __PACKET_SHAKE_H__
#define __PACKET_SHAKE_H__

#include "../firmware.h"

/**
 * @function
 * @ingroup shake
 *
 * Calculate the 128-bit Hash of an Input Data Buffer and Input Key Buffer
 * using the SHAKE-128 Extensible Hash Function.
 *
 * @param input Input data buffer to calculated
 * @param input_length Length of the input data buffer
 * @param key Input key buffer to calculated
 * @param key_length Length of the input key buffer
 * @param output Pointer to 128-bit Output buffer
 */
void Packet_sign_shake128(uint8_t *input, uint32_t input_length, uint8_t *key, uint32_t key_length, uint8_t *output);

// state context
typedef struct {
    union {                                 // state:
        uint8_t b[200];                     // 8-bit bytes
        uint64_t q[25];                     // 64-bit words
    } st;
    int pt, rsiz, mdlen;                    // these don't overflow
} sha3_ctx_t;

/**
 * @function
 * @ingroup shake
 *
 * Initialises a SHA3 context object
 *
 * @param c Pointer to pre-allocated SHA3 context object
 */
int shake128_init(sha3_ctx_t *c);

/**
 * @function
 * @ingroup shake
 *
 * Adds input data to a SHA3 context
 *
 * @param c Pointer to allocated & initialised SHA3 context object
 * @param data Input data buffer
 * @param len Length of the input data buffer
 */
int shake_update(sha3_ctx_t *c, const void *data, size_t len);

/**
 * @function
 * @ingroup shake
 *
 * Performs a SHAKE XOF Hash operation on SHA3 context
 *
 * @param c Pointer to allocated & initialised SHA3 context object
 */
void shake_xof(sha3_ctx_t *c);

/**
 * @function
 * @ingroup shake
 *
 * Exports requested length of output of SHA3 context
 *
 * @param c Pointer to allocated & initialised SHA3 context object
 * @param out Output data buffer
 * @param len Length of the output data buffer
 */
void shake_out(sha3_ctx_t *c, void *out, size_t len);

#endif /* __PACKET_SHAKE_H__ */