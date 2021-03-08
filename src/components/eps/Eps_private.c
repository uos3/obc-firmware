/**
 * @file Eps_private.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief EPS Component Module
 * 
 * Task ref: [UT_2.10.7]
 * 
 * See Eps_public.h for detailed description.
 * 
 * @version 0.1
 * @date 2021-02-18
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "components/eps/Eps_public.h"
#include "components/eps/Eps_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void Eps_build_uart_header(
    Eps_UartDataType data_type_in,
    uint8_t *p_header_out
) {
    /* Set the frame number */
    p_header_out[EPS_UART_HEADER_FRAME_NUMBER_POS] = DP.EPS.UART_FRAME_NUMBER;

    /* Set the data type */
    p_header_out[EPS_UART_HEADER_DATA_TYPE_POS] = (uint8_t)data_type_in;

    /* Increment the frame number, wrapping at 255 to 1. */
    if (DP.EPS.UART_FRAME_NUMBER >= 255) {
        DP.EPS.UART_FRAME_NUMBER = 1;
    }
    else {
        DP.EPS.UART_FRAME_NUMBER++;
    }
}

void Eps_append_crc_to_frame(
    uint8_t *p_frame_in,
    size_t length_without_crc_in
) {
    Crypto_Crc16 crc;

    /* Calculate the CRC of the frame header + payload */
    Crypto_get_crc16(
        p_frame_in,
        length_without_crc_in,
        &crc
    );

    /* Add the CRC to the end of the frame */
    p_frame_in[length_without_crc_in] = (uint8_t)((crc >> 8) & 0xFF);
    p_frame_in[length_without_crc_in + 1] = (uint8_t)(crc & 0xFF);
}

bool Eps_check_uart_frame(
    uint8_t *p_frame_in,
    size_t length_in
) {
    Crypto_Crc16 new_crc;

    /* Recompute the CRC for the frame, excluding the CRC bytes which are at
     * the end */
    Crypto_get_crc16(
        p_frame_in,
        length_in - EPS_UART_CRC_LENGTH,
        &new_crc
    );

    /* Check that the new CRC and the old CRC match */
    return (
        (((new_crc >> 8) & 0xFF) == p_frame_in[length_in - 2])
        &&
        ((new_crc & 0xFF) == p_frame_in[length_in - 1])
    );
}