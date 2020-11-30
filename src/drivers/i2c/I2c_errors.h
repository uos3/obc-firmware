/**
 * @ingroup i2c
 * @file I2c_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Defines error codes for the I2C module.
 * 
 * Task ref: [UT_2.8.7]
 * 
 * @version 0.1
 * @date 2020-11-30
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_I2C_ERRORS_H
#define H_I2C_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The I2C driver has not been initialised.
 */
#define I2C_ERROR_NOT_INITIALISED ((ErrorCode)MOD_ID_I2C | 1)

/**
 * @brief Failed to enable an I2C peripheral.
 */
#define I2C_ERROR_I2C_PERIPH_ENABLE_FAILED ((ErrorCode)MOD_ID_I2C | 2)

/**
 * @brief Failed to enable a GPIO peripheral.
 * 
 */
#define I2C_ERROR_GPIO_PERIPH_ENABLE_FAILED ((ErrorCode)MOD_ID_I2C | 3)

/**
 * @brief Indicates that no action has been queued for the given device
 */
#define I2C_ERROR_NO_ACTION_FOR_DEVICE ((ErrorCode)MOD_ID_I2C | 4)

/**
 * @brief Attempted to check the TivaWare error status of an action which
 * is of type NONE. 
 */
#define I2C_ERROR_CHECK_ON_NONE_ACTION ((ErrorCode)MOD_ID_I2C | 5)

/**
 * @brief Error indicating that a user attempted to read recieved bytes
 * from a device that had sent data. 
 */
#define I2C_ERROR_GET_RECVED_BYTES_FROM_SEND ((ErrorCode)MOD_ID_I2C | 6)

/**
 * @brief Attempted to get the recieved bytes from a non successful action.
 * Use I2c_get_device_action_status to check that the action was successful
 * before calling I2c_get_device_recved_bytes.
 */
#define I2C_ERROR_GET_RECVED_BYTES_ON_NON_SUCCESS_ACTION ((ErrorCode)MOD_ID_I2C | 7)

/**
 * @brief Attempted to send 0 bytes.
 */
#define I2C_ERROR_ZERO_LENGTH_SEND ((ErrorCode)MOD_ID_I2C | 8)

/**
 * @brief Failed to allocate memory for send action. 
 */
#define I2C_ERROR_MEMORY_ALLOC_FOR_SEND_FAILED ((ErrorCode)MOD_ID_I2C | 9)

/**
 * @brief Failed to allocate memory for recieve action. 
 */
#define I2C_ERROR_MEMORY_ALLOC_FOR_RECV_FAILED ((ErrorCode)MOD_ID_I2C | 10)

/**
 * @brief A new action could not be queued as the maximum number of actions
 * has been reached. 
 */
#define I2C_ERROR_MAX_ACTIONS_REACHED ((ErrorCode)MOD_ID_I2C | 11)

/**
 * @brief A new action could not be queued as the maximum number of actions
 * has been reached. 
 */
#define I2C_ERROR_ZERO_LENGTH_RECEIVE ((ErrorCode)MOD_ID_I2C | 12)

/**
 * @brief An unexpected action step index was found while executing an
 * action.
 */
#define I2C_ERROR_UNEXPECTED_ACTION_STEP ((ErrorCode)MOD_ID_I2C | 13)

/**
 * @brief An unexpected action step index was found while executing an
 * action.
 */
#define I2C_ERROR_UNEXPECTED_ACTION_SUBSTEP ((ErrorCode)MOD_ID_I2C | 14)

/**
 * @brief A master I2C module is busy.
 */
#define I2C_ERROR_MODULE_MASTER_BUSY ((ErrorCode)MOD_ID_I2C | 15)

/**
 * @brief Failed to acknowledge the address of an I2C device, is the device
 * address correct?
 */
#define I2C_ERROR_ADDRESS_ACK_FAILED ((ErrorCode)MOD_ID_I2C | 16)

/**
 * @brief Failed to acknowledge data sent to the master. The slave could be
 * unresponsive. 
 */
#define I2C_ERROR_DATA_ACK_FAILED ((ErrorCode)MOD_ID_I2C | 17)

/**
 * @brief The arbitration of the bus has been lost. This could indicate
 * that a slave is in error. 
 */
#define I2C_ERROR_ARBITRATION_LOST ((ErrorCode)MOD_ID_I2C | 18)

/**
 * @brief An unkown TivaWare library error has occured 
 */
#define I2C_ERROR_UNKNOWN_TIVAWARE_ERROR ((ErrorCode)MOD_ID_I2C | 19)

/**
 * @brief Error in the event manager.
 */
#define I2C_ERROR_EVENTMANAGER_ERROR ((ErrorCode)MOD_ID_I2C | 20)

/**
 * @brief The action cannot be queued as the module (bus) the device is on
 * is already locked by another device. 
 */
#define I2C_ERROR_MODULE_LOCKED_BY_ANOTHER_DEVICE ((ErrorCode)MOD_ID_I2C | 21)

#endif /* H_I2C_ERRORS_H */