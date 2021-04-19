/**
 * @file Eps_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief EPS Component Module error definitions
 * 
 * Task ref: [UT_2.10.7]
 * 
 * @version 0.1
 * @date 2021-02-17
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_EPS_ERRORS_H
#define H_EPS_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* Internal includes */
#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * ERRORS
 * ------------------------------------------------------------------------- */

/**
 * @brief An Eps function was called while the component is not iniailised.
 */
#define EPS_ERROR_NOT_INITIALISED ((ErrorCode)(MOD_ID_EPS | 1))

/**
 * @brief The Eps module state is invalid, i.e. not one of the `EPS_STATE_x`
 * enums. 
 */
#define EPS_ERROR_INVALID_STATE ((ErrorCode)(MOD_ID_EPS | 2))

/**
 * @brief Attempted to send a command to the EPS when the Eps module wasn't in
 * the IDLE state.
 */
#define EPS_ERROR_SEND_TC_WHEN_NOT_IDLE ((ErrorCode)(MOD_ID_EPS | 3))

/**
 * @brief Indicates an error occured with the event manager.
 */
#define EPS_ERROR_EVENTMANAGER_ERROR ((ErrorCode)(MOD_ID_EPS | 4))

/**
 * @brief The CRC of the request frame to be sent is incorrect. The frame will
 * be dropped and the user must try again.
 */
#define EPS_ERROR_INVALID_REQUEST_CRC ((ErrorCode)(MOD_ID_EPS | 5))

/**
 * @brief The CRC of the reply frame sent by the EPS is incorrect. The frame 
 * will be dropped and the user must send their request again.
 */
#define EPS_ERROR_INVALID_REPLY_CRC ((ErrorCode)(MOD_ID_EPS | 6))

/**
 * @brief Indicates that an error occured while calling Uart_recv_bytes.
 */
#define EPS_ERROR_UART_START_RECV_FAILED ((ErrorCode)(MOD_ID_EPS | 7))

/**
 * @brief Indicates that an error occured while calling Uart_send_bytes.
 */
#define EPS_ERROR_UART_START_SEND_FAILED ((ErrorCode)(MOD_ID_EPS | 8))

/**
 * @brief Indicates that an unexpected frame number was recieved while waiting
 * for a reply from the EPS.
 */
#define EPS_ERROR_UNEXPECTED_UART_FRAME_NUMBER ((ErrorCode)(MOD_ID_EPS | 9))

/**
 * @brief Indicates that the data type of the recieved frame was unexpected.
 */
#define EPS_ERROR_UNEXPECTED_UART_DATA_TYPE ((ErrorCode)(MOD_ID_EPS | 10))

/**
 * @brief Indicates that an unexpected frame was recieved.
 */
#define EPS_ERROR_RECV_UNEXPECTED_REPLY ((ErrorCode)(MOD_ID_EPS | 11))

/**
 * @brief Got the incorrect reply data type for the last request. 
 */
#define EPS_ERROR_INCORRECT_REPLY_DATA_TYPE ((ErrorCode)(MOD_ID_EPS | 12))

/**
 * @brief Indicates that the recieved HK data frame couldn't be parsed.
 */
#define EPS_ERROR_INVALID_HK_DATA ((ErrorCode)(MOD_ID_EPS | 13))

/**
 * @brief Indicates that the received loaded config doesn't match the config
 * which was sent to the EPS.
 */
#define EPS_ERROR_INCORRECT_LOADED_CONFIG ((ErrorCode)(MOD_ID_EPS | 14))

/**
 * @brief Indicates that the received OCP state doesn't match the one that was
 * requested.
 */
#define EPS_ERROR_INCORRECT_OCP_STATE ((ErrorCode)(MOD_ID_EPS | 15))

/**
 * @brief Indicates that when attempting to compare the request and reply data
 * types the request data type was invalid.
 */
#define EPS_ERROR_INVALID_REQUEST_DATA_TYPE ((ErrorCode)(MOD_ID_EPS | 16))

/**
 * @brief Indicates an error occured while using the timer driver.
 */
#define EPS_ERROR_TIMER_ERROR ((ErrorCode)(MOD_ID_EPS | 17))

/**
 * @brief Indicates that the EPS failed to respond to the command within the
 * required timeout.
 */
#define EPS_ERROR_COMMAND_TIMEOUT ((ErrorCode)(MOD_ID_EPS | 18))

#endif /* H_EPS_ERRORS_H */