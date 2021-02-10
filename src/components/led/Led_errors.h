/**
 * @ingroup gpio
 * @file Led_errors.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Defines error codes for the LED module.
 * 
 * @version 0.1
 * @date 2021-02-08
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_LED_ERRORS_H
#define H_LED_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The LED ID number was invalid (should be 0 or 1)
 */
#define LED_ERROR_INVALID_LED_ID ((ErrorCode)(MOD_ID_LED | 1))

#endif /* H_LED_ERRORS_H */