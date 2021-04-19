/**
 * @file Eps_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief EPS Component Module DataPool structure definition.
 * 
 * Task ref: [UT_2.10.7]
 * 
 * @version 0.1
 * @date 2021-02-15
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_EPS_DP_STRUCT_H
#define H_EPS_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "system/kernel/Kernel_errors.h"
#include "components/eps/Eps_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

typedef struct _Eps_Dp {

    /**
     * @brief Flag indicating whether or not the Eps has been initialised.
     * 
     * @dp 1
     */
    bool INITIALISED;

    /**
     * @brief Stores errors that occur during operation.
     * 
     * @dp 2
     */
    Error ERROR;

    /**
     * @brief The current state of the Eps module.
     * 
     * @dp 3
     */
    Eps_State STATE;

    /**
     * @brief Indicates if the configuration of the EPS is synchronised
     * (matches with) the config specified in the OBC's config file.
     * 
     * @dp 4
     */
    bool CONFIG_SYNCED;

    /**
     * @brief Flag indicating that there's a new request to send to the EPS.
     * 
     * TODO: remove
     * @dp 5
     */
    bool NEW_REQUEST;

    /**
     * @brief The request (command) to be sent in EPS_STATE_REQUEST.
     * 
     * PRIVATE: Do not set this value directly, instead use the public
     * functions. 
     * 
     * @dp 6
     */
    uint8_t EPS_REQUEST[EPS_MAX_UART_FRAME_LENGTH];

    /**
     * @brief Length of the request stored in DP.EPS.EPS_REQUEST.
     * 
     * @dp 7
     */
    size_t EPS_REQUEST_LENGTH;

    /**
     * @brief The reply from the EPS.
     * 
     * PRIVATE: Do not set this value directly, instead use the public
     * functions.
     * 
     * @dp 8
     */
    uint8_t EPS_REPLY[EPS_MAX_UART_FRAME_LENGTH];

    /**
     * @brief Length of the reply stored in DP.EPS.EPS_REPLY
     * 
     * @dp 9
     */
    size_t EPS_REPLY_LENGTH;

    /**
     * @brief Frame number of the latest UART frame to be sent.
     * 
     * @dp 10
     */
    uint8_t UART_FRAME_NUMBER;

    /**
     * @brief The status of the most recently sent command.
     * 
     * When the user has finished processing this flag it is their
     * responsibility to clear the value (set to EPS_COMMAND_STATUS_NONE).
     * 
     * @dp 11
     */
    Eps_CommandStatus COMMAND_STATUS;

    /**
     * @brief Most up-to-date housekeeping data returned by the EPS.
     * 
     * @dp 12
     */
    Eps_HkData HK_DATA;

    /**
     * @brief Stores errors coming from the UART driver.
     * 
     * @dp 13
     */
    Error UART_ERROR;

    /**
     * @brief Flag which is true if the Eps expects the next recieved bytes on
     * the UART to be a frame header. If false the next bytes should be data
     * associated with the recieved header.
     * 
     * @dp 14
     */
    bool EXPECT_HEADER;

    /**
     * @brief Indicates which OCP rails have been tripped, associated with the
     * EVT_EPS_OCP_RAIL_TRIPPED event, and the
     * EPS_UART_DATA_TYPE_TM_OCP_TRIPPED telemetry packet from the EPS.
     * 
     * @dp 15
     */
    Eps_OcpState TRIPPED_OCP_RAILS;

    /**
     * @brief Contains the reported OCP state of the EPS.
     * 
     * @dp 16
     */
    Eps_OcpState REPORTED_OCP_STATE;

    /**
     * @brief Event fired when a command timesout.
     * 
     * @dp 17
     */
    Event TIMEOUT_EVENT;

    /**
     * @brief Holds errors originating from the Timer driver.
     * 
     * @dp 18
     */
    Error TIMER_ERROR;

} Eps_Dp;

#endif /* H_EPS_DP_STRUCT_H */