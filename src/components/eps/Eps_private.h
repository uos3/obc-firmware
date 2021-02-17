/**
 * @file Eps_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief EPS Component Module private definitions.
 * 
 * Task ref: [UT_2.10.7]
 * 
 * @version 0.1
 * @date 2021-02-17
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_EPS_PRIVATE_H
#define H_EPS_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* Internal incldues */
#include "components/eps/Eps_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Maximum number of queued EPS commands.
 */
#define EPS_MAX_QUEUED_COMMANDS (8)

/* -------------------------------------------------------------------------   
 * TYPES
 * ------------------------------------------------------------------------- */

/**
 * @brief The byte representing the state of the EPS OCP (over-current 
 * protection) rails.
 * 
 * This is a bitfield, in which each bit represents the state of a particular
 * rail of the EPS, either on (1) or off (0). The bitmasks are defined as
 * EPS_OCP_RAIL_x_MASK. 
 */
typedef uint8_t Eps_OcpByte;

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Bitmask for the Radio TX rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 1.
 */
#define EPS_OCP_RAIL_RADIO_TX_MASK (0x1)

/**
 * @brief Bitmask for the Radio RX rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 2.
 */
#define EPS_OCP_RAIL_RADIO_RX_MASK (0x2)

/**
 * @brief Bitmask for the Camera rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 3.
 */
#define EPS_OCP_RAIL_CAMERA_MASK (0x4)

/**
 * @brief Bitmask for the OBC rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 4.
 */
#define EPS_OCP_RAIL_OBC_MASK (0x8)

/**
 * @brief Bitmask for the GNSS RX rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 5.
 */
#define EPS_OCP_RAIL_GNSS_RX_MASK (0x16)

/**
 * @brief Bitmask for the GNSS LNA (Low Noise Amplifier) rail of an 
 * Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 6.
 */
#define EPS_OCP_RAIL_GNSS_LNA_MASK (0x32)

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Build a new UART header from the given data type.
 * 
 * The frame number and data length are determined automatically.
 * 
 * @param data_type_in The data type of the frame to construct.
 * @param p_header_out Pointer to the location to store the header.
 */
void Eps_build_uart_header(
    Eps_UartDataType data_type_in,
    uint8_t *p_header_out
);

#endif /* H_EPS_PRIVATE_H */