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

    /* Set the payload length. Use pragma to disable the warning about not
     * using all the enum variants, we can't have a TM packet here. */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (data_type_in) {
        case EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA:
            p_header_out[EPS_UART_HEADER_PAYLOAD_LENGTH_POS]
                = EPS_UART_TC_COLLECT_HK_DATA_PL_LENGTH;
            break;
        
        case EPS_UART_DATA_TYPE_TC_SET_OCP_STATE:
            p_header_out[EPS_UART_HEADER_PAYLOAD_LENGTH_POS]
                = EPS_UART_TC_SET_OCP_STATE_PL_LENGTH;
            break;

        case EPS_UART_DATA_TYPE_TC_SEND_BATT_CMD:
            p_header_out[EPS_UART_HEADER_PAYLOAD_LENGTH_POS]
                = EPS_UART_TC_SEND_BATT_CMD_PL_LENGTH;
            break;

        default:
            DEBUG_ERR("Unimplmented payload length for Eps UART type %d", data_type_in);
            break;
    }
    #pragma GCC diagnostic pop

    /* Increment the frame number, wrapping at 255 to 0. */
    if (DP.EPS.UART_FRAME_NUMBER >= 255) {
        DP.EPS.UART_FRAME_NUMBER = 0;
    }
    else {
        DP.EPS.UART_FRAME_NUMBER++;
    }
}