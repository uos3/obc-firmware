/**
 * @file Power_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application private header
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_POWER_PRIVATE_H
#define H_POWER_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "applications/power/Power_public.h"
#include "components/eps/Eps_public.h"
#include "applications/mission/Mission_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Number of bits to for each OPMODE in the OpMode-OCP state config
 * vector. 
 */
#define POWER_OPMODE_OCP_CONFIG_NUM_BITS (4)

/**
 * @brief Position of the Radio TX rail in the 4 bits associated with an OpMode
 * config in the configuration vector.
 */
#define POWER_OPMODE_OCP_CONFIG_RADIO_TX_RAIL (0)

/**
 * @brief Position of the Radio RX/Camera rail in the 4 bits associated with an
 * OpMode config in the configuration vector.
 */
#define POWER_OPMODE_OCP_CONFIG_RADIO_RX_CAMERA_RAIL (1)

/**
 * @brief Position of the GNSS RX rail in the 4 bits associated with an OpMode
 * config in the configuration vector.
 */
#define POWER_OPMODE_OCP_CONFIG_GNSS_RX_RAIL (2)

/**
 * @brief Position of the GNSS LNA rail in the 4 bits associated with an OpMode
 * config in the configuration vector.
 */
#define POWER_OPMODE_OCP_CONFIG_GNSS_LNA_RAIL (3)

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Get the OCP state for the given OpMode from the configuration vector.
 * 
 * @param config_in The configuration vector to parse.
 * @param op_mode_in The OpMode to access inside the config vector.
 * @return Eps_OcpState The parsed OCP state.
 */
Eps_OcpState Power_get_ocp_state_for_op_mode(
    Power_OpModeOcpStateConfig config_in,
    Mission_OpMode op_mode_in
);

#endif /* H_POWER_PRIVATE_H */