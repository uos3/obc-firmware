/**
 * @file Eps_public.c
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

bool Eps_init(void) {
    /* Initialise the datapool to zero */
    memset(&DP.EPS, 0, sizeof(Eps_Dp));

    /* TODO: Initialise the UART */

    /* Set first state as init */
    DP.EPS.STATE = EPS_STATE_INIT_SEND_CFG;

    return true;
}

bool Eps_step(void) {
    /* If we're not initialise (and not in an init state) warn the user.
     * Technically not an error because the step functions should be callable 
     * when not iniailised, but this should be visible during testing. */
    if (!DP.EPS.INITIALISED 
        && 
        !((DP.EPS.STATE == EPS_STATE_INIT_SEND_CFG)
        ||
        (DP.EPS.STATE == EPS_STATE_INIT_WAIT_CFG_REPLY))
    ) {
        DEBUG_WRN("Eps is not initialised or in one of the initialisation states.");
        return true;
    }

    /* Main state machine */
    switch (DP.EPS.STATE) {
        case EPS_STATE_INIT_SEND_CFG:
            /* TODO: Serialise config and send over UART */

            /* Advance to the next state */
            DP.EPS.STATE = EPS_STATE_INIT_WAIT_CFG_REPLY;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case EPS_STATE_INIT_WAIT_CFG_REPLY:
            /* TODO: Wait for reply from EPS over UART */

            /* TODO: Compare the recieved config to the one we keep, to check
             * the EPS loaded it correctly. */

            /* Set the initialised flag */
            DP.EPS.INITIALISED = true;

            /* Advance to the next state */
            DP.EPS.STATE = EPS_STATE_IDLE;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case EPS_STATE_IDLE:

            /* Check for new request to send */
            if (DP.EPS.NEW_REQUEST) {
                /* If there is one move to the sending request state */
                DP.EPS.STATE = EPS_STATE_REQUEST;
            }
            /* Otherwise break out of the switch to return */
            else {
                break;
            }

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case EPS_STATE_REQUEST:

            /* TODO: Send the EPS request over the UART */

            /* Unset the new request flag */
            DP.EPS.NEW_REQUEST = false;

            /* Advance to next state */
            DP.EPS.STATE = EPS_STATE_WAIT_REPLY;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case EPS_STATE_WAIT_REPLY:

            /* TODO: Wait for reply from EPS over UART */

            /* TODO: Raise the completed event */

            /* TODO: If the reply is as expected set the status to OK, if not 
             * set the status to failure and raise the failed event. */

            /* TODO: Set the replied data in the DP */

            /* Return to idle. */
            DP.EPS.STATE = EPS_STATE_IDLE;

            /* No more progress to make this step, so break out */
            break;
        default:
            DEBUG_ERR("Invalid Eps state %d", DP.EPS.STATE);
            DP.EPS.ERROR_CODE = EPS_ERROR_INVALID_STATE;
            return false;
    }

    return true;
}

bool Eps_collect_hk_data(void) {
    uint8_t request_frame[EPS_MAX_UART_FRAME_LENGTH];
    size_t length 
        = EPS_UART_HEADER_LENGTH + EPS_UART_TC_COLLECT_HK_DATA_PL_LENGTH;
    
    /* Check Eps is initialised. Unlike the step function it is an error to
     * call this function before the EPS init sequence is finished. */
    if (!DP.EPS.INITIALISED) {
        DEBUG_ERR("Cannot send new command when Eps is not initialised");
        DP.EPS.ERROR_CODE = EPS_ERROR_NOT_INITIALISED;
        return false;
    }

    /* Check we are in idle mode, as can only issue new commands in idle */
    if (DP.EPS.STATE != EPS_STATE_IDLE) {
        DEBUG_ERR(
            "Cannot issue TC_COLLECT_HK_DATA to EPS as Eps is not in the IDLE state"
        );
        DP.EPS.ERROR_CODE = EPS_ERROR_SEND_TC_WHEN_NOT_IDLE;
        return false;
    }

    /* Build the header for a new COLLECT_HK request */
    Eps_build_uart_header(
        EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA,
        request_frame
    );

    /* There's no payload data for the COLLECT_HK command, so no need for
     * anything else. */

    /* Set the frame in the datapool */
    memcpy(
        (void* )DP.EPS.EPS_REQUEST, 
        (void *)request_frame, 
        length
    );

    /* Set the length of the request */
    DP.EPS.EPS_REQUEST_LENGTH = length;

    /* Set the new request flag */
    DP.EPS.NEW_REQUEST = true;

    return true;
}

bool Eps_set_ocp_state(Eps_OcpState ocp_state_in) {
    uint8_t request_frame[EPS_MAX_UART_FRAME_LENGTH];
    uint8_t state_byte = 0;
    size_t length 
        = EPS_UART_HEADER_LENGTH + EPS_UART_TC_SET_OCP_STATE_PL_LENGTH;

    /* Check Eps is initialised. Unlike the step function it is an error to
     * call this function before the EPS init sequence is finished. */
    if (!DP.EPS.INITIALISED) {
        DEBUG_ERR("Cannot send new command when Eps is not initialised");
        DP.EPS.ERROR_CODE = EPS_ERROR_NOT_INITIALISED;
        return false;
    }

    /* Check we are in idle mode, as can only issue new commands in idle */
    if (DP.EPS.STATE != EPS_STATE_IDLE) {
        DEBUG_ERR(
            "Cannot issue TC_COLLECT_HK_DATA to EPS as Eps is not in the IDLE state"
        );
        DP.EPS.ERROR_CODE = EPS_ERROR_SEND_TC_WHEN_NOT_IDLE;
        return false;
    }

    /* Build the header for a new SET_OCP_STATE request */
    Eps_build_uart_header(
        EPS_UART_DATA_TYPE_TC_SET_OCP_STATE,
        request_frame
    );

    /* Pack the state down into the single byte */
    state_byte |= (uint8_t)(
        (uint8_t)ocp_state_in.radio_tx << EPS_OCP_RAIL_RADIO_TX_SHIFT
    );
    state_byte |= (uint8_t)(
        (uint8_t)ocp_state_in.radio_rx << EPS_OCP_RAIL_RADIO_RX_SHIFT
    );
    state_byte |= (uint8_t)(
        (uint8_t)ocp_state_in.camera << EPS_OCP_RAIL_CAMERA_SHIFT
    );
    state_byte |= (uint8_t)(
        (uint8_t)ocp_state_in.obc << EPS_OCP_RAIL_OBC_SHIFT
    );
    state_byte |= (uint8_t)(
        (uint8_t)ocp_state_in.gnss_rx << EPS_OCP_RAIL_GNSS_RX_SHIFT
    );
    state_byte |= (uint8_t)(
        (uint8_t)ocp_state_in.gnss_lna << EPS_OCP_RAIL_GNSS_LNA_SHIFT
    );

    /* Set the next byte in the frame to the state byte, the payload of this
     * particular command */
    request_frame[EPS_UART_HEADER_LENGTH] = state_byte;

    /* Set the frame in the datapool */
    memcpy(
        (void* )DP.EPS.EPS_REQUEST, 
        (void *)request_frame, 
        length
    );

    /* Set the length of the request */
    DP.EPS.EPS_REQUEST_LENGTH = length;

    /* Set the new request flag */
    DP.EPS.NEW_REQUEST = true;

    return true;
}

bool Eps_send_battery_command(Eps_BattCmd batt_cmd_in) {
    uint8_t request_frame[EPS_MAX_UART_FRAME_LENGTH];
    size_t length
        = EPS_UART_HEADER_LENGTH + EPS_UART_TC_SEND_BATT_CMD_PL_LENGTH;

    /* Check Eps is initialised. Unlike the step function it is an error to
     * call this function before the EPS init sequence is finished. */
    if (!DP.EPS.INITIALISED) {
        DEBUG_ERR("Cannot send new command when Eps is not initialised");
        DP.EPS.ERROR_CODE = EPS_ERROR_NOT_INITIALISED;
        return false;
    }

    /* Check we are in idle mode, as can only issue new commands in idle */
    if (DP.EPS.STATE != EPS_STATE_IDLE) {
        DEBUG_ERR(
            "Cannot issue TC_COLLECT_HK_DATA to EPS as Eps is not in the IDLE state"
        );
        DP.EPS.ERROR_CODE = EPS_ERROR_SEND_TC_WHEN_NOT_IDLE;
        return false;
    }

    /* Build the header for a new SET_OCP_STATE request */
    Eps_build_uart_header(
        EPS_UART_DATA_TYPE_TC_SEND_BATT_CMD,
        request_frame
    );

    /* Set the next two bytes from the type and value of the batt cmd */
    request_frame[EPS_UART_HEADER_LENGTH] = batt_cmd_in.type;
    request_frame[EPS_UART_HEADER_LENGTH + 1] = batt_cmd_in.value;

    /* Set the frame in the datapool */
    memcpy(
        (void* )DP.EPS.EPS_REQUEST, 
        (void *)request_frame, 
        length
    );

    /* Set the length of the request */
    DP.EPS.EPS_REQUEST_LENGTH = length;

    /* Set the new request flag */
    DP.EPS.NEW_REQUEST = true;

    return true;
}