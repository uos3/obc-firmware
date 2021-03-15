/**
 * @ingroup uart
 * 
 * @file Gpio_public.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public header for the UART driver.
 * 
 * @version 0.1
 * @date 2020-12-06
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_UART_ERRORS_H
#define H_UART_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdint.h>

#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief TODO: The UART ID number was greater than the number of UARTs.
 */
#define UART_ERROR_MAX_NUM_UARTS ((ErrorCode)MOD_ID_UART | 1)

/**
 * @brief TODO: The UART peripheral failed to enable after reaching the maximum
 *  number of checks.
 */
#define UART_ERROR_PERIPHERAL_READY_FAILED ((ErrorCode)MOD_ID_UART | 2)

/**
 * @brief TODO: A function has been called on the UART before it has been
 * initialised.
 */
#define UART_ERROR_NOT_INITIALISED ((ErrorCode)MOD_ID_UART | 3)

/**
 * @brief TODO: There was no space available in the TX FIFO, so the character
 * could not be sent.
 */
#define UART_ERROR_PUT_CHAR_FAILED ((ErrorCode)MOD_ID_UART | 4)

#endif /* H_UART_ERRORS_H */