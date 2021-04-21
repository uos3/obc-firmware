/**
 * @file Packing_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Utility module to assist in packing of bytes to and from byte streams.
 * 
 * @version 0.1
 * @date 2021-04-21
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* Internal includes */
#include "util/packing/Packing_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

uint16_t Packing_u16_from_be(uint8_t *p_data_in) {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
    return (uint16_t)((uint16_t)(p_data_in[0] << 8) | p_data_in[1]);
    #else
    return (uint16_t)((uint16_t)(p_data_in[1] << 8) | p_data_in[0]);
    #endif
}

uint16_t Packing_u16_from_le(uint8_t *p_data_in) {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
    return (uint16_t)((uint16_t)(p_data_in[1] << 8) | p_data_in[0]);
    #else
    return (uint16_t)((uint16_t)(p_data_in[0] << 8) | p_data_in[1]);
    #endif
}