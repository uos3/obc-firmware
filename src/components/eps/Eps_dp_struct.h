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
    ErrorCode ERROR_CODE;

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
     * @brief The request (command) to be sent in EPS_STATE_REQUEST.
     * 
     * PRIVATE: Do not set this value directly, instead use the public
     * functions. 
     * 
     * @dp 5
     */
    uint8_t EPS_REQUEST[EPS_MAX_UART_FRAME_LENGTH];

    /**
     * @brief The reply from the EPS.
     * 
     * PRIVATE: Do not set this value directly, instead use the public
     * functions.
     * 
     * @dp 6
     */
    uint8_t EPS_REPLY[EPS_MAX_UART_FRAME_LENGTH];

    /**
     * @brief Frame number of the latest UART frame to be sent.
     * 
     * @dp 7
     */
    uint8_t UART_FRAME_NUMBER;

    /**
     * @brief The status of the most recently sent command.
     * 
     * @dp 8
     */
    Eps_CommandStatus COMMAND_STATUS;

    /**
     * @brief Most up-to-date housekeeping data returned by the EPS.
     * 
     * @dp 9
     */
    Eps_HkData HK_DATA;


} Eps_Dp;

#endif /* H_EPS_DP_STRUCT_H */