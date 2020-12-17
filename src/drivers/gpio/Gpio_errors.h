/**
 * @ingroup gpio
 * @file Gpio_errors.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Defines error codes for the GPIO module.
 * 
 * @version 0.1
 * @date 2020-12-06
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_GPIO_ERRORS_H
#define H_GPIO_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The GPIO peripheral failed to enable.
 */
#define GPIO_ERROR_PERIPHERAL_ENABLE_FAILED ((ErrorCode)MOD_ID_I2C | 1)

/**
 * @brief The GPIO mode was neither GPIO_MODE_INPUT or GPIO_MODE_OUTPUT
 */
#define GPIO_ERROR_UNEXPECTED_MODE ((ErrorCode)MOD_ID_I2C | 2)

/**
 * @brief The GPIO id number was greater than the total number of GPIOs
 */
#define GPIO_ERROR_EXCEEDED_NUM_GPIOS ((ErrorCode)MOD_ID_I2C | 3)

/**
 * @brief An action was attempted on the GPIO when it has not been initialised.
 */
#define GPIO_ERROR_NOT_INITIALISED ((ErrorCode)MOD_ID_I2C | 4)

#endif /* H_GPIO_ERRORS_H */