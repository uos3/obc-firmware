/**
 * @defgroup pn9
 *
 * @brief PN9 Routines
 *
 * @details 
 *
 * @ingroup util
 */

#ifndef __UTIL_PN9_H__
#define __UTIL_PN9_H__

#include "../firmware.h"

/**
 * @function
 * @ingroup pn9
 *
 * XOR a byte buffer with the 
 * PN9 sequence used in the CC1120.
 *
 * @param buffer Pointer to the buffer to process
 * @param size Size of the buffer
 */
void Util_pn9(uint8_t *buffer, uint32_t length);

#endif /* __UTIL_PN9_H__ */