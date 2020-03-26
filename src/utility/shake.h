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
 * @ingroup util
 */

#ifndef __UTIL_SHAKE_H__
#define __UTIL_SHAKE_H__

// #include "../firmware.h"
#include <stdint.h>

/* Shake Context */
typedef struct {
    union {                                 // state:
        uint8_t b[200];                     // 8-bit bytes
        uint64_t q[25];                     // 64-bit words
    } st;
    int pt, rsiz;                    // these don't overflow
    uint32_t mdlen;
} util_shake_ctx_t;

/**
 * @function
 * @ingroup shake
 *
 * Initialises a SHA3 context object
 *
 * @param c Pointer to pre-allocated SHA3 context object
 * @param output_length Length in bytes of intended Hash Function output
 */
void Util_shake_init(util_shake_ctx_t *c, uint8_t output_length);

/**
 * @function
 * @ingroup shake
 *
 * Adds input data to a SHA3 context
 *
 * @param c Pointer to allocated & initialised SHA3 context object
 * @param data Input data buffer
 * @param data_length Length of the input data buffer
 */
void Util_shake_update(util_shake_ctx_t *c, const uint8_t *data, uint32_t data_length);

/**
 * @function
 * @ingroup shake
 *
 * Produces output of SHA3 context.
 *
 * Note: This should only be run once for an initialised context object.
 *
 * @param c Pointer to allocated & initialised SHA3 context object
 * @param out Output data buffer
 */
void Util_shake_out(util_shake_ctx_t *c, uint8_t *out);

#endif /* __UTIL_SHAKE_H__ */