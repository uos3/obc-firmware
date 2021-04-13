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
/*#include "drivers/uart/Uart_public.h"
#include "drivers/udma/Udma_public.h"*/
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "components/eps/Eps_public.h"
#include "components/eps/Eps_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Eps_init(void) {

    /* UART shall be initialised before calling this function */

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

            /* Check UART for unsolicited TM */

            /* Check for new request to send */
            if (EventManager_poll_event(EVT_EPS_NEW_REQUEST)) {
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
                DP.EPS.ERROR.code = EPS_ERROR_INVALID_REQUEST_CRC;
                DP.EPS.ERROR.p_cause = NULL;

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
                     * ERROR.p_cause and without returning false here */
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
                DP.EPS.ERROR.code = EPS_ERROR_EVENTMANAGER_ERROR;
                DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
                /* TODO: return to IDLE here and set command as failed? */
                return false;
            }

            /* TODO: If the reply is as expected set the status to OK, if not 
             * set the status to failure. */
            DP.EPS.COMMAND_STATUS = EPS_COMMAND_SUCCESS;

            /* TODO: Set the replied data in the DP */

            /* TODO: If the request was HK raise the new HK event
             * FIXME: This is a debugging hack to send the event if the request
             * was for HK, this shouldn't be done for real. */
            if (DP.EPS.EPS_REQUEST[EPS_UART_HEADER_DATA_TYPE_POS] 
                == 
                EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA
            ) {
                if (!EventManager_raise_event(EVT_EPS_NEW_HK_DATA)) {
                    DEBUG_ERR("EventManager error while raising new HK event");
                    DP.EPS.ERROR.code = EPS_ERROR_EVENTMANAGER_ERROR;
                    DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
                    /* TODO: return to IDLE here and set command as failed? */
                    return false;
                }
            }

            /* Return to idle. */
            DP.EPS.STATE = EPS_STATE_IDLE;

            /* No more progress to make this step, so break out */
            break;
        default:
            DEBUG_ERR("Invalid Eps state %d", DP.EPS.STATE);
            DP.EPS.ERROR.code = EPS_ERROR_INVALID_STATE;
            DP.EPS.ERROR.p_cause = NULL;
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
        DP.EPS.ERROR.code = EPS_ERROR_NOT_INITIALISED;
        DP.EPS.ERROR.p_cause = NULL;
        return false;
    }

    /* Check we are in idle mode, as can only issue new commands in idle */
    if (DP.EPS.STATE != EPS_STATE_IDLE) {
        DEBUG_ERR(
            "Cannot issue TC_COLLECT_HK_DATA to EPS as Eps is not in the IDLE state"
        );
        DP.EPS.ERROR.code = EPS_ERROR_SEND_TC_WHEN_NOT_IDLE;
        DP.EPS.ERROR.p_cause = NULL;
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
        DP.EPS.ERROR.code = EPS_ERROR_EVENTMANAGER_ERROR;
        DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
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
        DP.EPS.ERROR.code = EPS_ERROR_NOT_INITIALISED;
        DP.EPS.ERROR.p_cause = NULL;
        return false;
    }

    /* Check we are in idle mode, as can only issue new commands in idle */
    if (DP.EPS.STATE != EPS_STATE_IDLE) {
        DEBUG_ERR(
            "Cannot issue TC_COLLECT_HK_DATA to EPS as Eps is not in the IDLE state"
        );
        DP.EPS.ERROR.code = EPS_ERROR_SEND_TC_WHEN_NOT_IDLE;
        DP.EPS.ERROR.p_cause = NULL;
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
        DP.EPS.ERROR.code = EPS_ERROR_EVENTMANAGER_ERROR;
        DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
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
        DP.EPS.ERROR.code = EPS_ERROR_NOT_INITIALISED;
        DP.EPS.ERROR.p_cause = NULL;
        return false;
    }

    /* Check we are in idle mode, as can only issue new commands in idle */
    if (DP.EPS.STATE != EPS_STATE_IDLE) {
        DEBUG_ERR(
            "Cannot issue TC_SET_OCP_STATE to EPS as Eps is not in the IDLE state"
        );
        DP.EPS.ERROR.code = EPS_ERROR_SEND_TC_WHEN_NOT_IDLE;
        DP.EPS.ERROR.p_cause = NULL;
        return false;
    }

    /* Build the header for a new SET_OCP_STATE request */
    Eps_build_uart_header(
        EPS_UART_DATA_TYPE_TC_SET_OCP_STATE,
        request_frame
    );

    /* Pack the state down into the single byte. */
    state_byte = Eps_ocp_state_to_ocp_byte(ocp_state_in);

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
        DP.EPS.ERROR.code = EPS_ERROR_EVENTMANAGER_ERROR;
        DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
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
        DP.EPS.ERROR.code = EPS_ERROR_NOT_INITIALISED;
        DP.EPS.ERROR.p_cause = NULL;
        return false;
    }

    /* Check we are in idle mode, as can only issue new commands in idle */
    if (DP.EPS.STATE != EPS_STATE_IDLE) {
        DEBUG_ERR(
            "Cannot issue TC_SEND_BATT_CMD to EPS as Eps is not in the IDLE state"
        );
        DP.EPS.ERROR.code = EPS_ERROR_SEND_TC_WHEN_NOT_IDLE;
        DP.EPS.ERROR.p_cause = NULL;
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
        DP.EPS.ERROR.code = EPS_ERROR_EVENTMANAGER_ERROR;
        DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
        return false;
    }

    return true;
}

bool Eps_reset_ocp(Eps_OcpState ocp_state_in) {
    uint8_t request_frame[EPS_MAX_UART_FRAME_LENGTH];
    uint8_t state_byte = 0;
    size_t length_without_crc
        = EPS_UART_HEADER_LENGTH + EPS_UART_TC_RESET_OCP_PL_LENGTH;
    size_t length_with_crc = length_without_crc + EPS_UART_CRC_LENGTH;

    /* Check Eps is initialised. Unlike the step function it is an error to
     * call this function before the EPS init sequence is finished. */
    if (!DP.EPS.INITIALISED) {
        DEBUG_ERR("Cannot send new command when Eps is not initialised");
        DP.EPS.ERROR.code = EPS_ERROR_NOT_INITIALISED;
        DP.EPS.ERROR.p_cause = NULL;
        return false;
    }

    /* Check we are in idle mode, as can only issue new commands in idle */
    if (DP.EPS.STATE != EPS_STATE_IDLE) {
        DEBUG_ERR(
            "Cannot issue TC_RESET_OCP to EPS as Eps is not in the IDLE state"
        );
        DP.EPS.ERROR.code = EPS_ERROR_SEND_TC_WHEN_NOT_IDLE;
        DP.EPS.ERROR.p_cause = NULL;
        return false;
    }

    /* Build the header for a new SET_OCP_STATE request */
    Eps_build_uart_header(
        EPS_UART_DATA_TYPE_TC_RESET_OCP,
        request_frame
    );

    /* Pack the state down into the single byte. */
    state_byte = Eps_ocp_state_to_ocp_byte(ocp_state_in);

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
        DP.EPS.ERROR.code = EPS_ERROR_EVENTMANAGER_ERROR;
        DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
        return false;
    }

    return true;
}