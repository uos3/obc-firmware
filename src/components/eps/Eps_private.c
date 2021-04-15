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

bool Eps_parse_hk_data(
    uint8_t *p_data_in,
    Eps_HkData *p_hk_data_out
) {
    uint8_t byte_idx = 0;
    /* Strategy: Consume bytes sequentially parsing each group according to the
     * format specified in [SW_ICD 5.2.4.4]. */

    /* OCP STATE */
    p_hk_data_out->ocp_state = Eps_ocp_byte_to_ocp_state(p_data_in[byte_idx]);
    byte_idx += 1;

    /* BATTERY STATUS */
    p_hk_data_out->batt_status = Eps_parse_batt_status(&p_data_in[byte_idx]);
    byte_idx += 2;

    /* BATTERY VOLTAGE
     *
     * Noting big endian, so first byte is the least significant, second is
     * most significant.
     */
    p_hk_data_out->batt_voltage_volts 
        = Eps_adc_voltage_sense_scaledint_to_volts(
            (p_data_in[byte_idx] | (p_data_in[byte_idx + 1] << 8))
        );
    byte_idx += 2;
}

double Eps_adc_voltage_sense_scaledint_to_volts(
    uint16_t voltage_scaledint_in
) {
    /* Conversion equation comes from Register doc
     * 
     * TODO: ref ICD */
    return (
        (
            (double)voltage_scaledint_in 
            * 
            EPS_ADC_VOLTAGE_SENSE_SCALEDINT_TO_VOLTS
        )
        *
        (
            (EPS_ADC_VOLTAGE_SENSE_R1_OHMS + EPS_ADC_VOLTAGE_SENSE_R2_OHMS)
            /
            EPS_ADC_VOLTAGE_SENSE_R2_OHMS
        )  
    );
}

double Eps_adc_voltage_sense_scaledint_to_amps(
    uint16_t voltage_scaledint_in,
    double shunt_resistance_ohms_in
) {
    /* Conversion equation comes from Register doc
     * 
     * TODO: ref ICD */
    return (
        (
            (double)voltage_scaledint_in 
            *
            EPS_ADC_VOLTAGE_SENSE_SCALEDINT_TO_VOLTS
        )
        * shunt_resistance_ohms_in 
        * 100.0
    );
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
        EPS_OCP_RAIL_OBC_MASK;
    state.gnss_lna = 
        (byte_in & EPS_OCP_RAIL_GNSS_LNA_MASK) 
        == 
        EPS_OCP_RAIL_GNSS_LNA_MASK;

    return state;
}

Eps_OcpByte Eps_ocp_state_to_ocp_byte(Eps_OcpState state_in) {
    Eps_OcpByte byte;

    byte |= (uint8_t)(
        (uint8_t)state_in.radio_tx << EPS_OCP_RAIL_RADIO_TX_SHIFT
    );
    byte |= (uint8_t)(
        (uint8_t)state_in.radio_rx_camera 
            << EPS_OCP_RAIL_RADIO_RX_CAMERA_SHIFT
    );
    byte |= (uint8_t)(
        (uint8_t)state_in.eps_mcu << EPS_OCP_RAIL_EPS_MCU_SHIFT
    );
    byte |= (uint8_t)(
        (uint8_t)state_in.obc << EPS_OCP_RAIL_OBC_SHIFT
    );
    byte |= (uint8_t)(
        (uint8_t)state_in.gnss_rx << EPS_OCP_RAIL_GNSS_RX_SHIFT
    );
    byte |= (uint8_t)(
        (uint8_t)state_in.gnss_lna << EPS_OCP_RAIL_GNSS_LNA_SHIFT
    );

    return byte;
}

Eps_BattStatus Eps_parse_batt_status(uint8_t *p_data_in) {
    Eps_BattStatus status = {0};

    /* TODO */

    return status;
}