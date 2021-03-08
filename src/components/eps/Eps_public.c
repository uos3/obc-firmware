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

    /* Set first state as idle */
    DP.EPS.STATE = EPS_STATE_IDLE;

    /* Set the initial frame number to 1. Frame number 0 is reserved for
     * unsolicited TM packets */
    DP.EPS.UART_FRAME_NUMBER = 1;

    /* Set the EPS as initialised */
    DP.EPS.INITIALISED = true;

    return true;
}

bool Eps_step(void) {
    bool is_event_raised = false;
    #ifdef DEBUG_MODE
    char p_hex_str[512] = "";
    #endif

    /* If we're not initialised warn the user.
     * Technically not an error because the step functions should be callable 
     * when not iniailised, but this should be visible during testing.
     * TODO: is this true? */
    if (!DP.EPS.INITIALISED) {
        DEBUG_WRN("Eps is not initialised or in one of the initialisation states.");
        return true;
    }

    /* Main state machine */
    switch (DP.EPS.STATE) {
        case EPS_STATE_IDLE:

            /* Check for new request to send */
            if (!EventManager_poll_event(
                EVT_EPS_NEW_REQUEST, 
                &is_event_raised
            )) {
                DEBUG_ERR("EventManager error while polling new request event");
                DP.EPS.ERROR_CODE = EPS_ERROR_EVENTMANAGER_ERROR;
                return false;
            }
            if (is_event_raised) {
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

            /* Check that the CRC in the frame to be sent is correct */
            if (!Eps_check_uart_frame(
                DP.EPS.EPS_REQUEST, 
                DP.EPS.EPS_REQUEST_LENGTH
            )) {
                DEBUG_ERR(
                    "CRC of request frame to be sent is incorrect, dropping"
                );
                DP.EPS.ERROR_CODE = EPS_ERROR_INVALID_REQUEST_CRC;
                /* Move the state back to IDLE, we are dropping the request
                 * here and the user must try again. Signal this with the
                 * completed event but the failed status. */
                if (!EventManager_raise_event(EVT_EPS_COMMAND_COMPLETE)) {
                    DEBUG_ERR(
                        "EventManager error while raising command complete event"
                    );
                    /* An error during an error is bad, but we don't want to
                     * hide the fact that the root cause was the invalid CRC,
                     * so allow the path to continue here without changing
                     * ERROR_CODE and without returning false here */
                }
                DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
                DP.EPS.STATE = EPS_STATE_IDLE;
                return false;
            }

            /* TODO: Send the EPS request over the UART */

            /* Print the message */
            #ifdef DEBUG_MODE
            Debug_hex_string(
                DP.EPS.EPS_REQUEST, 
                p_hex_str, 
                DP.EPS.EPS_REQUEST_LENGTH
            );
            DEBUG_DBG("EPS message = %s", p_hex_str);
            p_hex_str[0] = '\0';
            #endif

            /* Advance to next state */
            DP.EPS.STATE = EPS_STATE_WAIT_REPLY;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case EPS_STATE_WAIT_REPLY:

            /* TODO: Wait for reply from EPS over UART */

            /* Raise the completed event */
            if (!EventManager_raise_event(EVT_EPS_COMMAND_COMPLETE)) {
                DEBUG_ERR("EventManager error while raising command complete event");
                DP.EPS.ERROR_CODE = EPS_ERROR_EVENTMANAGER_ERROR;
                /* TODO: return to IDLE here and set command as failed? */
                return false;
            }

            /* TODO: If the reply is as expected set the status to OK, if not 
             * set the status to failure. */
            DP.EPS.COMMAND_STATUS = EPS_COMMAND_SUCCESS;

            /* TODO: Set the replied data in the DP */

            /* TODO: If the request was HK raise the new HK event */

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

bool Eps_send_config(void) {
    uint8_t request_frame[EPS_MAX_UART_FRAME_LENGTH];
    size_t length_without_crc 
        = EPS_UART_HEADER_LENGTH + EPS_UART_TC_SET_CONFIG_PL_LENGTH;
    size_t length_with_crc = length_without_crc + EPS_UART_CRC_LENGTH;

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

    /* Build the header for a new SET_CONFIG request */
    Eps_build_uart_header(
        EPS_UART_DATA_TYPE_TC_SET_CONFIG,
        request_frame
    );

    /* TODO: build the config struct and add it to the frame */

    /* Add the CRC to the frame */
    Eps_append_crc_to_frame(
        request_frame,
        length_without_crc
    );

    /* Set the frame in the datapool */
    memcpy(
        (void *)DP.EPS.EPS_REQUEST, 
        (void *)request_frame, 
        length_with_crc
    );

    /* Set the length of the request */
    DP.EPS.EPS_REQUEST_LENGTH = length_with_crc;
    
    /* Set that the status is in progress, so new commands can't be raised */
    DP.EPS.COMMAND_STATUS = EPS_COMMAND_IN_PROGRESS;

    /* Raise the new request event */
    if (!EventManager_raise_event(EVT_EPS_NEW_REQUEST)) {
        DEBUG_ERR("EventManager error while raising new request event");
        DP.EPS.ERROR_CODE = EPS_ERROR_EVENTMANAGER_ERROR;
        return false;
    }

    return true;
}

bool Eps_collect_hk_data(void) {
    uint8_t request_frame[EPS_MAX_UART_FRAME_LENGTH];
    size_t length_without_crc
        = EPS_UART_HEADER_LENGTH + EPS_UART_TC_COLLECT_HK_DATA_PL_LENGTH;
    size_t length_with_crc = length_without_crc + EPS_UART_CRC_LENGTH;

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

    /* Add the CRC to the frame */
    Eps_append_crc_to_frame(
        request_frame,
        length_without_crc
    );

    /* Set the frame in the datapool */
    memcpy(
        (void *)DP.EPS.EPS_REQUEST, 
        (void *)request_frame, 
        length_with_crc
    );

    /* Set the length of the request */
    DP.EPS.EPS_REQUEST_LENGTH = length_with_crc;

    /* Set that the status is in progress, so new commands can't be raised */
    DP.EPS.COMMAND_STATUS = EPS_COMMAND_IN_PROGRESS;

    /* Raise the new request event */
    if (!EventManager_raise_event(EVT_EPS_NEW_REQUEST)) {
        DEBUG_ERR("EventManager error while raising new request event");
        DP.EPS.ERROR_CODE = EPS_ERROR_EVENTMANAGER_ERROR;
        return false;
    }

    return true;
}

bool Eps_set_ocp_state(Eps_OcpState ocp_state_in) {
    uint8_t request_frame[EPS_MAX_UART_FRAME_LENGTH];
    uint8_t state_byte = 0;
    size_t length_without_crc
        = EPS_UART_HEADER_LENGTH + EPS_UART_TC_SET_OCP_STATE_PL_LENGTH;
    size_t length_with_crc = length_without_crc + EPS_UART_CRC_LENGTH;

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

    /* Pack the state down into the single byte. The ordering here is
     * important, and follows the numerical ordering of the rails, which is
     * defined by the EPS_OCP_RAIL_x_SHIFT values. */
    state_byte |= (uint8_t)(
        (uint8_t)ocp_state_in.radio_tx << EPS_OCP_RAIL_RADIO_TX_SHIFT
    );
    state_byte |= (uint8_t)(
        (uint8_t)ocp_state_in.radio_rx_camera 
            << EPS_OCP_RAIL_RADIO_RX_CAMERA_SHIFT
    );
    state_byte |= (uint8_t)(
        (uint8_t)ocp_state_in.eps_mcu << EPS_OCP_RAIL_EPS_MCU_SHIFT
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

    /* Add the CRC to the frame */
    Eps_append_crc_to_frame(
        request_frame,
        length_without_crc
    );

    /* Set the frame in the datapool */
    memcpy(
        (void *)DP.EPS.EPS_REQUEST, 
        (void *)request_frame, 
        length_with_crc
    );

    /* Set the length of the request */
    DP.EPS.EPS_REQUEST_LENGTH = length_with_crc;

    /* Set that the status is in progress, so new commands can't be raised */
    DP.EPS.COMMAND_STATUS = EPS_COMMAND_IN_PROGRESS;

    /* Raise the new request event */
    if (!EventManager_raise_event(EVT_EPS_NEW_REQUEST)) {
        DEBUG_ERR("EventManager error while raising new request event");
        DP.EPS.ERROR_CODE = EPS_ERROR_EVENTMANAGER_ERROR;
        return false;
    }

    return true;
}

bool Eps_send_battery_command(Eps_BattCmd batt_cmd_in) {
    uint8_t request_frame[EPS_MAX_UART_FRAME_LENGTH];
    size_t length_without_crc
        = EPS_UART_HEADER_LENGTH + EPS_UART_TC_SEND_BATT_CMD_PL_LENGTH;
    size_t length_with_crc = length_without_crc + EPS_UART_CRC_LENGTH;

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

    /* Add the CRC to the frame */
    Eps_append_crc_to_frame(
        request_frame,
        length_without_crc
    );

    /* Set the frame in the datapool */
    memcpy(
        (void *)DP.EPS.EPS_REQUEST, 
        (void *)request_frame, 
        length_with_crc
    );

    /* Set the length of the request */
    DP.EPS.EPS_REQUEST_LENGTH = length_with_crc;

    /* Set that the status is in progress, so new commands can't be raised */
    DP.EPS.COMMAND_STATUS = EPS_COMMAND_IN_PROGRESS;

    /* Raise the new request event */
    if (!EventManager_raise_event(EVT_EPS_NEW_REQUEST)) {
        DEBUG_ERR("EventManager error while raising new request event");
        DP.EPS.ERROR_CODE = EPS_ERROR_EVENTMANAGER_ERROR;
        return false;
    }

    return true;
}