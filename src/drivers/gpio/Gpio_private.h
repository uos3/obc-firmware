/**
 * @ingroup gpio
 * 
 * @file Gpio_private.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Private header for the GPIO driver.
 * 
 * @version 0.1
 * @date 2020-12-07
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_GPIO_PRIVATE_H
#define H_GPIO_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "drivers/gpio/Gpio_public.h"
#include "drivers/gpio/Gpio_errors.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/* Maximum number of programmable GPIOs set by TivaWare manual */
#define GPIO_MAX_NUM_GPIOS (43)

/* Maximum number of times to check that the GPIO peripheral is ready
 * TODO: This is arbitrary and based off of the current I2C peripheral
 * ready checks. */
#define GPIO_MAX_NUM_PERIPHERAL_READY_CHECKS (10)

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialises a GPIO
 * 
 * @param p_gpio_modules_in Pointer to an array of GPIO module to be initialised.
 * @param num_gpio_modules_in Number of gpio modules to initialise
 * @param mode Mode of the GPIO (input or output)
 * @return ErrorCode 
 */
ErrorCode Gpio_init(uint32_t *p_gpio_modules_in, size_t num_gpio_modules_in, Gpio_Mode mode);

#endif /* H_GPIO_PRIVATE_H */