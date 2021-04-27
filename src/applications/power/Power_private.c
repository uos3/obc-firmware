/**
 * @file Power_private.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application private implementation
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/opmode_manager/OpModeManager_public.h"
#include "applications/power/Power_public.h"
#include "applications/power/Power_private.h"
#include "components/eps/Eps_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

Eps_OcpState Power_get_ocp_state_for_op_mode(
    Power_OpModeOcpStateConfig config_in,
    OpModeManager_OpMode op_mode_in
) {
    Eps_OcpState state;
    uint32_t lsb_shift;

    /* Set the EPS and OBC rails on, as they must always be on for this to
     * work. */
    state.eps_mcu = true;
    state.obc = true;

    /* 
     * Get the position of the LSB associated with this OPMODE in the vector.
     * 
     * This is based on the known definitions of the OPMODE indexes, with the
     * lowest significant bit being located at 4 * the OPMODE index (0-based)
     */
    lsb_shift = POWER_OPMODE_OCP_CONFIG_NUM_BITS * (uint32_t)(op_mode_in);

    /* Each rail is located at a particular bit shift above the LSB, so we just
     * read those to determine if a rail should be enabled. */
    state.radio_tx = config_in 
        & (uint32_t)(1 << (lsb_shift + POWER_OPMODE_OCP_CONFIG_RADIO_TX_RAIL));
    state.radio_rx_camera = config_in 
        & (uint32_t)(
            1 << (lsb_shift + POWER_OPMODE_OCP_CONFIG_RADIO_RX_CAMERA_RAIL)
        );
    state.gnss_rx = config_in 
        & (uint32_t)(1 << (lsb_shift + POWER_OPMODE_OCP_CONFIG_GNSS_RX_RAIL));
    state.gnss_lna = config_in 
        &(uint32_t) (1 << (lsb_shift + POWER_OPMODE_OCP_CONFIG_GNSS_LNA_RAIL));

    return state;
}

bool Power_low_power_status_check(void) {
    /* Check that the EPS reported battery voltage sense (note: not actual
     * battery voltage, it's the voltage of the sense resistor!), is above the
     * minimum threshold stored in the config file */
    if (DP.EPS.HK_DATA.vbatt_vsense 
        <= 
        CFG.POWER_VBATT_VSENSE_LOW_POWER_THRESHOLD
    ) {
        DEBUG_ERR("Battery voltage sense below LP threshold");
        DEBUG_INF(
            "CFG.POWER_VBATT_VSENSE_LOW_POWER_THRESHOLD is %u, vbatt_vsense is %u",
            CFG.POWER_VBATT_VSENSE_LOW_POWER_THRESHOLD,
            DP.EPS.HK_DATA.vbatt_vsense
        );

        /* Set low power status */
        DP.POWER.LOW_POWER_STATUS = POWER_LOW_POWER_STATUS_LOW_POWER;
    }
    else {
        DEBUG_INF("Battery voltage sense above LP threshold");
        DEBUG_INF(
            "CFG.POWER_VBATT_VSENSE_LOW_POWER_THRESHOLD is %u, vbatt_vsense is %u",
            CFG.POWER_VBATT_VSENSE_LOW_POWER_THRESHOLD,
            DP.EPS.HK_DATA.vbatt_vsense
        );

        DP.POWER.LOW_POWER_STATUS = POWER_LOW_POWER_STATUS_OK;
    }
}