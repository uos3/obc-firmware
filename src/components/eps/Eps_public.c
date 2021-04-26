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
#include "drivers/uart/Uart_public.h"
#include "drivers/timer/Timer_public.h"
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
    #ifdef DEBUG_MODE
    char p_hex_str[512] = "";
    #endif

    /* If we're not initialised warn the user */
    if (!DP.EPS.INITIALISED) {
        DEBUG_WRN("Eps is not initialised");
        return true;
    }

    /* Check UART any TM */
    if (EventManager_poll_event(EVT_UART_EPS_RX_COMPLETE)) {
        DEBUG_DBG("RX event raised");
        /* If we expected a header for this recieve process it, this will start
         * the recieve for the payload and CRC as well */
        if (DP.EPS.EXPECT_HEADER) {
            if (!Eps_process_uart_header()) {
                DEBUG_ERR("Couldn't process header from EPS");
                return false;
            }
        }
        /* Otherwise we expected a payload, so we process that instead */
        else {
            if (!Eps_process_uart_payload()) {
                DEBUG_ERR("Couldn't process payload from EPS");
                return false;
            }
        }
    }

    /* Main state machine */
    switch (DP.EPS.STATE) {
        case EPS_STATE_IDLE:

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

            /* Send the EPS request over the UART */
            DP.EPS.UART_ERROR.code = Uart_send_bytes(
                UART_DEVICE_ID_EPS,
                (uint8_t *)DP.EPS.EPS_REQUEST,
                DP.EPS.EPS_REQUEST_LENGTH
            );
            if (DP.EPS.UART_ERROR.code != ERROR_NONE) {
                DEBUG_ERR("Error calling Uart_send_bytes for EPS");
                DP.EPS.UART_ERROR.p_cause = NULL;
                DP.EPS.ERROR.code = EPS_ERROR_UART_START_SEND_FAILED;
                DP.EPS.ERROR.p_cause = &DP.EPS.UART_ERROR;
                DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
                DP.EPS.STATE = EPS_STATE_IDLE;
                return false;
            }

            /* Start the timeout timer */
            DP.EPS.TIMER_ERROR.code = Timer_start_one_shot(
                EPS_COMMAND_TIMEOUT_S,
                &DP.EPS.TIMEOUT_EVENT
            );
            if (DP.EPS.TIMER_ERROR.code != ERROR_NONE) {
                DEBUG_ERR("Failed to start timeout timer");
                DP.EPS.TIMER_ERROR.p_cause = NULL;
                DP.EPS.ERROR.code = EPS_ERROR_TIMER_ERROR;
                DP.EPS.ERROR.p_cause = &DP.EPS.TIMER_ERROR;
                DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
                DP.EPS.STATE = EPS_STATE_IDLE;
                return false;
            }

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

            /* Prepare to recieve any potential unsolicited header bytes from the 
             * UART */
            DP.EPS.UART_ERROR.code = Uart_recv_bytes(
                UART_DEVICE_ID_EPS, 
                (uint8_t *)DP.EPS.EPS_REPLY, 
                EPS_UART_HEADER_LENGTH
            );
            if (DP.EPS.UART_ERROR.code != ERROR_NONE) {
                DEBUG_ERR("Unable to start Uart_recv_bytes for EPS");
                DP.EPS.UART_ERROR.p_cause = NULL;
                DP.EPS.ERROR.code = EPS_ERROR_UART_START_RECV_FAILED;
                DP.EPS.ERROR.p_cause = &DP.EPS.UART_ERROR;
                return false;
            }
            DP.EPS.EXPECT_HEADER = true;
            DEBUG_DBG("EPS reply recv started");

            /* Advance to next state */
            DP.EPS.STATE = EPS_STATE_WAIT_REPLY;

            /* Explicit fallthrough allowed here so that as much progress is
             * made as possible in a single cycle. */
            __attribute__ ((fallthrough));
        case EPS_STATE_WAIT_REPLY:

            /* In this state we are waiting for a reply we just sent, however
             * don't actually do any processing of it here. Instead it's
             * handled above with the poll for EVT_UART_EPS_RX_COMPLETE. This
             * is because we also need to handle possible unsolicited TM, and
             * it is better to handle all recieves in one place. The
             * Eps_process_uart_x() functions set COMMAND_STATUS to either
             * success or failure when we're ready to move out of this state.
             * 
             * In addition we check the timeout event here, that way if we
             * timeout we can handle it at this level */
            
            if (EventManager_poll_event(
                DP.EPS.TIMEOUT_EVENT
            )) {
                DEBUG_ERR("EPS command timed out");
                DP.EPS.COMMAND_STATUS = EPS_COMMAND_FAILURE;
                DP.EPS.ERROR.code = EPS_ERROR_COMMAND_TIMEOUT;
                DP.EPS.ERROR.p_cause = NULL;
                DP.EPS.STATE = EPS_STATE_IDLE;

                /* Raise command complete event */
                if (!EventManager_raise_event(EVT_EPS_COMMAND_COMPLETE)) {
                    DEBUG_ERR("Couldn't raise EPS command complete event");
                    DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
                }

                return false;
            }

            if (DP.EPS.COMMAND_STATUS != EPS_COMMAND_IN_PROGRESS) {
                DP.EPS.STATE = EPS_STATE_IDLE;

                /* Raise command complete event */
                if (!EventManager_raise_event(EVT_EPS_COMMAND_COMPLETE)) {
                    DEBUG_ERR("Couldn't raise EPS command complete event");
                    DP.EPS.ERROR.p_cause = &DP.EVENTMANAGER.ERROR;
                    return false;
                }

                /* Stop the timer */
                DP.EPS.TIMER_ERROR.code = Timer_disable(
                    DP.EPS.TIMEOUT_EVENT
                );
                if (DP.EPS.TIMER_ERROR.code != ERROR_NONE) {
                    DEBUG_ERR("Couldn't disable timeout timer");
                    /* This isn't technically a command failure, so we don't
                     * set that */
                    DP.EPS.TIMER_ERROR.p_cause = NULL;
                    DP.EPS.ERROR.code = EPS_ERROR_TIMER_ERROR;
                    DP.EPS.ERROR.p_cause = &DP.EPS.TIMER_ERROR;
                    DP.EPS.STATE = EPS_STATE_IDLE;
                    return false;
                }
            }

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
    uint8_t request_frame[EPS_MAX_UART_FRAME_LENGTH] = {0};
    size_t length_without_crc 
        = EPS_UART_HEADER_LENGTH + EPS_UART_TC_SET_CONFIG_PL_LENGTH;
    size_t length_with_crc = length_without_crc + EPS_UART_CRC_LENGTH;
    Eps_ConfigData config;

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

    /* build the config struct and add it to the frame */
    config.reset_rail_after_ocp = CFG.EPS_RESET_RAIL_AFTER_OCP;
    config.tobc_timer_length = CFG.EPS_TOBC_TIMER_LENGTH;

    Eps_serialise_config_data(
        &config,
        &request_frame[EPS_UART_HEADER_LENGTH]
    );

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
    DP.EPS.CONFIG_SYNCED = false;

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
    request_frame[EPS_UART_HEADER_LENGTH + 1] = batt_cmd_in.value_a;
    request_frame[EPS_UART_HEADER_LENGTH + 2] = batt_cmd_in.value_b;

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

bool Eps_do_ocp_states_match(
    Eps_OcpState *p_a_in,
    Eps_OcpState *p_b_in
) {
    return (
        (p_a_in->radio_tx == p_b_in->radio_tx)
        &&
        (p_a_in->radio_rx_camera == p_b_in->radio_rx_camera)
        &&
        (p_a_in->eps_mcu == p_b_in->eps_mcu)
        &&
        (p_a_in->obc == p_b_in->obc)
        &&
        (p_a_in->gnss_rx == p_b_in->gnss_rx)
        &&
        (p_a_in->gnss_lna == p_b_in->gnss_lna)
    );
}