/**
 * @ingroup uart
 * 
 * @file Gpio_public.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public header for the GPIO driver.
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
 * @brief TODO: This is a temporary errorcode to be replaced and filled with 
 * future errorcode defines.
 */
#define UART_ERROR_TEMP ((ErrorCode)MOD_ID_UART | 1)

#endif /* H_UART_ERRORS_H */