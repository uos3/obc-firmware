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
#include "util/packing/Packing_public.h"
#include "drivers/uart/Uart_public.h"
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

void Eps_parse_hk_data(
    uint8_t *p_data_in,
    Eps_HkData *p_hk_data_out
) {
    /* Strategy: Consume bytes sequentially parsing each group according to the
     * format specified in [SW_ICD 5.2.8]. */

    p_hk_data_out->batt_status = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->batt_output_voltage_scaledint = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->batt_current_magnitude_scaledint = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->batt_current_direction = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;
    
    p_hk_data_out->batt_motherboard_temp_scaledint = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->batt_5v_current_scaledint = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->batt_5v_voltage_scaledint = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->batt_3v3_current_scaledint = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->batt_3v3_voltage_scaledint = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->batt_daughterboard_temp_scaledint = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->batt_daughterboard_heater_status = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->eps_temp_scaledint = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_top1_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_top2_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp5_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp6_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp6_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp4_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp4_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp5_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp3_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp3_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->sys_5v_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->sys_3v3_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp2_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp2_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp1_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_north2_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_north1_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->charge_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_west1_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->mppt_bus_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->mppt2_lower_pv_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->mppt2_mid_pv_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_west2_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_south2_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_south2_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->uvp_5v_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->uvp_3v3_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->vbatt_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->ocp1_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_east2_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->pv_east1_csense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->mppt1_lower_pv_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->mppt3_lower_pv_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->mppt1_mid_pv_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->mppt3_mid_pv_vsense = Packing_u16_from_be(
        p_data_in
    );
    p_data_in += 2;

    p_hk_data_out->log_ocp1_trip_count = *p_data_in;
    p_data_in++;

    p_hk_data_out->log_ocp2_trip_count = *p_data_in;
    p_data_in++;

    p_hk_data_out->log_ocp3_trip_count = *p_data_in;
    p_data_in++;

    p_hk_data_out->log_ocp4_trip_count = *p_data_in;
    p_data_in++;

    p_hk_data_out->log_ocp5_trip_count = *p_data_in;
    p_data_in++;

    p_hk_data_out->log_ocp6_trip_count = *p_data_in;
    p_data_in++;

    p_hk_data_out->log_reboot_count = *p_data_in;
    p_data_in++;

    p_hk_data_out->log_tobc_time_count = *p_data_in;
    p_data_in++;

    p_hk_data_out->ocp_rail_state = *p_data_in;
    p_data_in++;
}

void Eps_parse_config_data(
    uint8_t *p_data_in,
    Eps_ConfigData *p_config_out
) {
    /* Strategy: consume bytes and pack them into the struct as done with the
     * hk data */
    p_config_out->reset_rail_after_ocp = *p_data_in;
    p_data_in++;

    p_config_out->tobc_timer_length = Packing_u16_from_be(p_data_in);
    p_data_in += 2;
}

void Eps_serialise_config_data(
    Eps_ConfigData *p_config_in, 
    uint8_t *p_data_out
) {
    *p_data_out = p_config_in->reset_rail_after_ocp;
    p_data_out++;

    Packing_u16_to_be(p_config_in->tobc_timer_length, p_data_out);
}

Eps_OcpState Eps_ocp_byte_to_ocp_state(Eps_OcpByte byte_in) {
    Eps_OcpState state = {0};

    state.radio_tx = 
        (byte_in & EPS_OCP_RAIL_RADIO_TX_MASK) 
        == 
        EPS_OCP_RAIL_RADIO_TX_MASK;
    state.radio_rx_camera = 
        (byte_in & EPS_OCP_RAIL_RADIO_RX_CAMERA_MASK) 
        == 
        EPS_OCP_RAIL_RADIO_RX_CAMERA_MASK;
    state.eps_mcu = 
        (byte_in & EPS_OCP_RAIL_EPS_MCU_MASK) 
        == 
        EPS_OCP_RAIL_EPS_MCU_MASK;
    state.obc = 
        (byte_in & EPS_OCP_RAIL_OBC_MASK) 
        == 
        EPS_OCP_RAIL_OBC_MASK;
    state.gnss_rx = 
        (byte_in & EPS_OCP_RAIL_GNSS_RX_MASK) 
        == 
        EPS_OCP_RAIL_GNSS_RX_MASK;
    state.gnss_lna = 
        (byte_in & EPS_OCP_RAIL_GNSS_LNA_MASK) 
        == 
        EPS_OCP_RAIL_GNSS_LNA_MASK;

    return state;
}

Eps_OcpByte Eps_ocp_state_to_ocp_byte(Eps_OcpState state_in) {
    Eps_OcpByte byte = 0;

    byte |= (Eps_OcpByte)(
        (uint8_t)state_in.radio_tx << EPS_OCP_RAIL_RADIO_TX_SHIFT
    );
    byte |= (Eps_OcpByte)(
        (uint8_t)state_in.radio_rx_camera 
            << EPS_OCP_RAIL_RADIO_RX_CAMERA_SHIFT
    );
    byte |= (Eps_OcpByte)(
        (uint8_t)state_in.eps_mcu << EPS_OCP_RAIL_EPS_MCU_SHIFT
    );
    byte |= (Eps_OcpByte)(
        (uint8_t)state_in.obc << EPS_OCP_RAIL_OBC_SHIFT
    );
    byte |= (Eps_OcpByte)(
        (uint8_t)state_in.gnss_rx << EPS_OCP_RAIL_GNSS_RX_SHIFT
    );
    byte |= (Eps_OcpByte)(
        (uint8_t)state_in.gnss_lna << EPS_OCP_RAIL_GNSS_LNA_SHIFT
    );

    return byte;
}

Eps_BattStatus Eps_parse_batt_status(uint8_t *p_data_in) {
    Eps_BattStatus status = {0};

    /* TODO */

    return status;
}

bool Eps_process_uart_header(void) {
    Uart_Status uart_status;

    /* We've recieved the header bytes, need to check with UART if it was
     * successful.
     * TODO: This function doesn't work as expected yet */
    /*Uart_get_status(UART_DEVICE_ID_EPS, &uart_status);*/

    /* TODO: Determine if UART worked or not */

    /* Check the frame number. If it's unsolicited TM (zero) or the expected
     * reply to a request (frame numbers match), we setup the read for the
     * frame's payload and CRC. */
    if ((DP.EPS.EPS_REPLY[EPS_UART_HEADER_FRAME_NUMBER_POS] == 0)
        ||
        (
            DP.EPS.EPS_REPLY[EPS_UART_HEADER_FRAME_NUMBER_POS]
            ==
            DP.EPS.EPS_REQUEST[EPS_UART_HEADER_FRAME_NUMBER_POS]
        )
    ) {
        DEBUG_TRC("Got EPS UART header");

        if (!Eps_start_uart_receive_payload()) {
            DEBUG_ERR("Couldn't start receive of payload");
            return false;
        }
    }
    /* If it doesn't match any of these it's an unexpected reply */
    else {
        /* If we're not currently in the wait for reply state we can chock this
         * up to an EPS side error, so we discard the frame and issue a
         * warning, raise the event for error monitoring */
        if (DP.EPS.STATE != EPS_STATE_WAIT_REPLY) {
            DEBUG_WRN(
                "EPS UART frame with non-zero frame number outside of wait reply state (frame number = %u)",
                DP.EPS.EPS_REPLY[EPS_UART_HEADER_FRAME_NUMBER_POS]
            );

            /* Clear the frame */
            memset(
                &DP.EPS.EPS_REPLY,
                0,
                EPS_MAX_UART_FRAME_LENGTH
            );
            DP.EPS.EPS_REPLY_LENGTH = 0;

            if (!EventManager_raise_event(
                EVT_EPS_RECEIVED_UNEXPECTED_UART_FRAME
            )) {
                DEBUG_WRN("Couldn't raise EVT_EPS_RECEIVED_UNEXPECTED_UART_FRAME");
                /* The system can continue fine if we don't raise this, so we
                 * don't bother making an error out of it */
            }
        }
        /* If it didn't match but we were expecting a reply we set this command
         * as failed */
        else {
            DEBUG_ERR(
                "Expected UART frame number %u but got %u",
                DP.EPS.EPS_REQUEST[EPS_UART_HEADER_FRAME_NUMBER_POS],
                DP.EPS.EPS_REPLY[EPS_UART_HEADER_FRAME_NUMBER_POS]
            );

            /* Set the command as a failure */
            DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
            DP.EPS.ERROR.code = EPS_ERROR_UNEXPECTED_UART_FRAME_NUMBER;
            DP.EPS.ERROR.p_cause = NULL;

            /* We have to actually recieve all the bytes we need still, since
             * those bytes will be blocking other replys coming through */
            if (!Eps_start_uart_receive_payload()) {
                DEBUG_ERR("Couldn't start receive of payload");
                return false;
            }
        }
    }
}

bool Eps_process_uart_payload(void) {
    Crypto_Crc16 expected_crc;
    Crypto_Crc16 received_crc;
    char p_hex_str[64];
    
    /* We've recieved the payload bytes, need to check with UART if it was
     * successful.
     * TODO: This function doesn't work as expected yet */
    /*Uart_get_status(UART_DEVICE_ID_EPS, &uart_status);*/

    /* TODO: Determine if UART worked or not */

    /* Print the message */
    #ifdef DEBUG_MODE
    Debug_hex_string(
        DP.EPS.EPS_REPLY, 
        p_hex_str, 
        DP.EPS.EPS_REPLY_LENGTH
    );
    DEBUG_DBG("message from EPS = %s", p_hex_str);
    p_hex_str[0] = '\0';
    #endif

    /* Calculate the expected CRC of the received packet */
    Crypto_get_crc16(
        (uint8_t *)DP.EPS.EPS_REPLY,
        DP.EPS.EPS_REPLY_LENGTH - EPS_UART_CRC_LENGTH,
        &expected_crc
    );

    /* Pack the received CRC from it's pair of bytes into a 16 bit int. */
    received_crc = Packing_u16_from_be(
        &DP.EPS.EPS_REPLY[DP.EPS.EPS_REPLY_LENGTH - EPS_UART_CRC_LENGTH]
    );

    /* If the CRCs don't match then we can't actually depend on the data type
     * or frame number being correct (this could actually be a problem in that
     * we might recieve the wrong number of bytes for this packet, although how
     * to deal with this is unclear). So we must decide now if the packet is
     * valid. If the CRCs don't match we will set any currently executing
     * command as failed, that way the Power app will retry the command, and we
     * can try to get another good reply from it */
    if (expected_crc != received_crc) {
        DEBUG_ERR(
            "CRC of frames don't match, got 0x%04X, expected 0x%04X, dropping frame",
            received_crc,
            expected_crc
        );

        DP.EPS.ERROR.code = EPS_ERROR_INVALID_REPLY_CRC;
        DP.EPS.ERROR.p_cause = NULL;
        
        /* If waiting for reply */
        if (DP.EPS.STATE == EPS_STATE_WAIT_REPLY) {
            /* Set the command as failed, this will prompt power to retry the
             * last command. */
            DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
        }

        /* Even though technically we had an error here we have cleared it up
         * and handled it, the step function should be allowed to continue */
        return true;
    }

    /* Check if the command failed at the header stage, i.e. if the frame
     * number was incorrect. This is signalled by the COMMAND_STATUS dp being
     * set to failure already, when it should be IN_PROGRESS instead. */
    if (DP.EPS.COMMAND_STATUS == EPS_COMMAND_FAILURE) {
        /* If the command is already a failure we will drop the packet, discard
         * the recieved bytes */
        DEBUG_TRC(
            "Dropping frame (data type = %u) with invalid frame number %u",
            DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS],
            DP.EPS.EPS_REPLY[EPS_UART_HEADER_FRAME_NUMBER_POS]
        );
        memset(
            &DP.EPS.EPS_REPLY,
            0,
            EPS_MAX_UART_FRAME_LENGTH
        );
        DP.EPS.EPS_REPLY_LENGTH = 0;

        return true;
    }

    /* Check the packet's frame number and process either unsolicited or normal
     * TM. If the frame number was incorrect then we have to raise an error. */
    if (DP.EPS.EPS_REPLY[EPS_UART_HEADER_FRAME_NUMBER_POS] == 0) {
        /* The only unsolicited TM we can expect is the OCP_TRIPPED packet, so
         * if it's not this type the packet has an invalid frame number, and we
         * can discard the packet as being invalid */
        if (DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]
            !=
            EPS_UART_DATA_TYPE_TM_OCP_TRIPPED
        ) {
            DEBUG_ERR(
                "Received unsolicited packet with wrong data type %u, dropping",
                DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]
            );
            memset(
                &DP.EPS.EPS_REPLY,
                0,
                EPS_MAX_UART_FRAME_LENGTH
            );
            DP.EPS.EPS_REPLY_LENGTH = 0;
            return true;
        }

        /* Store the payload data in the data pool */
        DP.EPS.TRIPPED_OCP_RAILS = Eps_ocp_byte_to_ocp_state(
            DP.EPS.EPS_REPLY[EPS_UART_HEADER_LENGTH]
        );
        
        DEBUG_ERR("EPS OCP rail tripped: %d%d%d%d%d%d",
            DP.EPS.TRIPPED_OCP_RAILS.radio_tx,
            DP.EPS.TRIPPED_OCP_RAILS.radio_rx_camera,
            DP.EPS.TRIPPED_OCP_RAILS.eps_mcu,
            DP.EPS.TRIPPED_OCP_RAILS.obc,
            DP.EPS.TRIPPED_OCP_RAILS.gnss_rx,
            DP.EPS.TRIPPED_OCP_RAILS.gnss_lna
        );

        /* Raise the event signaling the trip */
        if (!EventManager_raise_event(EVT_EPS_OCP_RAIL_TRIPPED)) {
            DEBUG_ERR("Couldn't raise EVT_EPS_OCP_RAIL_TRIPPED");
            DP.EPS.ERROR.code = EPS_ERROR_EVENTMANAGER_ERROR;
            DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
            return false;
        }

        return true;
    }
    /* Process normal TM from a command */
    else if (DP.EPS.EPS_REPLY[EPS_UART_HEADER_FRAME_NUMBER_POS]
        ==
        DP.EPS.EPS_REQUEST[EPS_UART_HEADER_FRAME_NUMBER_POS]
    ) {
        /* Check we are actully supposed to be recieving a command */
        if (DP.EPS.STATE != EPS_STATE_WAIT_REPLY) {
            DEBUG_ERR(
                "Received frame while not waiting for reply from the EPS"
            );
            DP.EPS.ERROR.code = EPS_ERROR_RECV_UNEXPECTED_REPLY;
            DP.EPS.ERROR.p_cause = NULL;

            /* Discard the frame */
            memset(
                &DP.EPS.EPS_REPLY,
                0,
                EPS_MAX_UART_FRAME_LENGTH
            );
            DP.EPS.EPS_REPLY_LENGTH = 0;

            return true;
        }

        /* Process the recieved reply, checking that it is correct when
         * compared to the request which was sent. */
        if (!Eps_process_reply()) {
            return false;
        }

        /* Wait reply will raise command complete event if required */
    }
    /* Frame number is wrong, handle this error */
    else {
        DEBUG_ERR(
            "Expected UART frame number %u or 0 but got %u",
            DP.EPS.EPS_REQUEST[EPS_UART_HEADER_FRAME_NUMBER_POS],
            DP.EPS.EPS_REPLY[EPS_UART_HEADER_FRAME_NUMBER_POS]
        );

        /* Set the error code */
        DP.EPS.ERROR.code = EPS_ERROR_UNEXPECTED_UART_FRAME_NUMBER;
        DP.EPS.ERROR.p_cause = NULL;

        /* Discard the frame */
        memset(
            &DP.EPS.EPS_REPLY,
            0,
            EPS_MAX_UART_FRAME_LENGTH
        );
        DP.EPS.EPS_REPLY_LENGTH = 0;

        /* If we were waiting for a reply to a TC mark the command as failed */
        if (DP.EPS.STATE == EPS_STATE_WAIT_REPLY) {
            DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
        }

        return true;
    }

    return true;
}

bool Eps_start_uart_receive_payload(void) {
    size_t payload_length = 0;

    /* Determine the payload length by comparing the data type in the existing
     * header.
     * NOTE: use pragma here so we don't get warning about not checking for TC
     * data types */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]) {
        case EPS_UART_DATA_TYPE_TM_BATT_REPLY:
            payload_length = EPS_UART_TM_BATT_REPLY_PL_LENGTH;
            break;
        case EPS_UART_DATA_TYPE_TM_HK_DATA:
            payload_length = EPS_UART_TM_HK_DATA_PL_LENGTH;
            break;
        case EPS_UART_DATA_TYPE_TM_LOADED_CONFIG:
            payload_length = EPS_UART_TM_LOADED_CONFIG_PL_LENGTH;
            break;
        case EPS_UART_DATA_TYPE_TM_OCP_STATE:
            payload_length = EPS_UART_TM_OCP_STATE_PL_LENGTH;
            break;
        case EPS_UART_DATA_TYPE_TM_OCP_TRIPPED:
            payload_length = EPS_UART_TM_OCP_TRIPPED_PL_LENGTH;
            break;
        default:
            DEBUG_ERR(
                "Received unexpected UART data type: %u",
                DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]
            );
            /* Don't return here, we need to setup the rx for the remaining
             * bytes */
    }
    #pragma GCC diagnostic pop

    /* Add the CRC to the length */
    payload_length += sizeof(Crypto_Crc16);

    /* Set the total expected reply length */
    DP.EPS.EPS_REPLY_LENGTH = EPS_UART_HEADER_LENGTH + payload_length;

    /* Call the recieve function */
    DP.EPS.UART_ERROR.code = Uart_recv_bytes(
        UART_DEVICE_ID_EPS,
        &DP.EPS.EPS_REPLY[EPS_UART_HEADER_LENGTH],
        payload_length
    );
    DP.EPS.UART_ERROR.p_cause = NULL;
    if (DP.EPS.UART_ERROR.code != ERROR_NONE) {
        DEBUG_ERR("Unable to start Uart_recv_bytes for EPS");
        DP.EPS.UART_ERROR.p_cause = NULL;
        DP.EPS.ERROR.code = EPS_ERROR_UART_START_RECV_FAILED;
        DP.EPS.ERROR.p_cause = &DP.EPS.UART_ERROR;
        return false;
    }
    DP.EPS.EXPECT_HEADER = false;

    return true;
}

bool Eps_process_reply(void) {
    Eps_HkData hk_data;
    
    switch (DP.EPS.EPS_REQUEST[EPS_UART_HEADER_DATA_TYPE_POS]) {
        case EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA:
            if (DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]
                ==
                EPS_UART_DATA_TYPE_TM_HK_DATA
            ) {
                /* Got an HK reply to an HK request, parse the HK and
                 * update it in the data pool. Use a temp variable so we
                 * don't overwrite the datapool if it fails to parse */
                Eps_parse_hk_data(
                    &DP.EPS.EPS_REPLY[EPS_UART_HEADER_LENGTH],
                    &hk_data
                );

                /* Set the data pool data and command status, and emmit
                    * the event signalling new HK data */
                DP.EPS.HK_DATA = hk_data;
                DP.EPS.COMMAND_STATUS = EPS_COMMAND_SUCCESS;

                if (!EventManager_raise_event(EVT_EPS_NEW_HK_DATA)) {
                    DEBUG_ERR("Couldn't raise EVT_EPS_NEW_HK_DATA");
                    DP.EPS.ERROR.code = EPS_ERROR_EVENTMANAGER_ERROR;
                    DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
                    return false;
                }
            }
            else {
                Eps_handle_incorrect_reply_data_type();
                return true;
            }
            break;

        case EPS_UART_DATA_TYPE_TC_SET_CONFIG:
            if (DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]
                ==
                EPS_UART_DATA_TYPE_TM_LOADED_CONFIG
            ) {
                /* Check that the reply payload matches the request payload */
                if (memcmp(
                    (void *)&DP.EPS.EPS_REQUEST[EPS_UART_HEADER_LENGTH],
                    (void *)&DP.EPS.EPS_REPLY[EPS_UART_HEADER_LENGTH],
                    (size_t)EPS_UART_TC_SET_CONFIG_PL_LENGTH
                ) != 0) {
                    DEBUG_ERR("Set and loaded EPS configs do not match");
                    DP.EPS.ERROR.code = EPS_ERROR_INCORRECT_LOADED_CONFIG;
                    DP.EPS.ERROR.p_cause = NULL;
                    DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
                    return true;
                }
                else {
                    /* Set the command as successful, and the config synced 
                     * flag */
                    DP.EPS.COMMAND_STATUS = EPS_COMMAND_SUCCESS;
                    DP.EPS.CONFIG_SYNCED = true;
                }
            }
            else {
                Eps_handle_incorrect_reply_data_type();
                return true;
            }
            break;

        case EPS_UART_DATA_TYPE_TC_SET_OCP_STATE:
            if (DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]
                ==
                EPS_UART_DATA_TYPE_TM_OCP_STATE
            ) {
                /* Check that the reply payload matches the request payload */
                if (DP.EPS.EPS_REQUEST[EPS_UART_HEADER_LENGTH]
                    !=
                    DP.EPS.EPS_REPLY[EPS_UART_HEADER_LENGTH]
                ) {
                    DEBUG_ERR(
                        "Returned OCP state (0x%X) doesn't match requested state (0x%X)",
                        DP.EPS.EPS_REPLY[EPS_UART_HEADER_LENGTH],
                        DP.EPS.EPS_REQUEST[EPS_UART_HEADER_LENGTH]
                    );
                    DP.EPS.ERROR.code = EPS_ERROR_INCORRECT_OCP_STATE;
                    DP.EPS.ERROR.p_cause = NULL;
                    DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
                    return true;
                }
                else {
                    /* Set the reported state and set command as successful */
                    DP.EPS.REPORTED_OCP_STATE = Eps_ocp_byte_to_ocp_state(
                        (Eps_OcpByte)DP.EPS.EPS_REPLY[EPS_UART_HEADER_LENGTH]
                    );
                    DP.EPS.COMMAND_STATUS = EPS_COMMAND_SUCCESS;
                }
            }
            else {
                Eps_handle_incorrect_reply_data_type();
                return true;
            }
            break;
        
        case EPS_UART_DATA_TYPE_TC_SEND_BATT_CMD:
            if (DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]
                ==
                EPS_UART_DATA_TYPE_TM_BATT_REPLY
            ) {
                /* Set the battery reply in the data pool */
                /* TODO */
                DP.EPS.COMMAND_STATUS = EPS_COMMAND_SUCCESS;
            }
            else {
                Eps_handle_incorrect_reply_data_type();
                return true;
            }
            break;

        case EPS_UART_DATA_TYPE_TC_RESET_OCP:
            if (DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]
                ==
                EPS_UART_DATA_TYPE_TM_OCP_STATE
            ) {
                /* Put the OCP payload into the DP and set the command as
                 * success, the return will be checked by the Power app */
                DP.EPS.REPORTED_OCP_STATE = Eps_ocp_byte_to_ocp_state(
                    (Eps_OcpByte)DP.EPS.EPS_REPLY[EPS_UART_HEADER_LENGTH]
                );
                DP.EPS.COMMAND_STATUS = EPS_COMMAND_SUCCESS;
            }
            else {
                Eps_handle_incorrect_reply_data_type();
                return true;
            }
            break;
        default:
            DEBUG_ERR(
                "Invalid EPS request data type when comparing against reply"
            );
            DP.EPS.ERROR.code = EPS_ERROR_INVALID_REQUEST_DATA_TYPE;
            DP.EPS.ERROR.p_cause = NULL;
            return false;
    }

    return true;
}

void Eps_handle_incorrect_reply_data_type(void) {
    /* Didn't get the right reply, set command as failed. */
    DEBUG_ERR(
        "Incorrect reply to last request, expected %u but got %u",
        EPS_UART_DATA_TYPE_TM_HK_DATA,
        DP.EPS.EPS_REPLY[EPS_UART_HEADER_DATA_TYPE_POS]
    );
    DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
    DP.EPS.ERROR.code = EPS_ERROR_INCORRECT_REPLY_DATA_TYPE;
    DP.EPS.ERROR.p_cause = NULL;

    /* Discard the frame */
    memset(
        &DP.EPS.EPS_REPLY,
        0,
        EPS_MAX_UART_FRAME_LENGTH
    );
    DP.EPS.EPS_REPLY_LENGTH = 0;
}