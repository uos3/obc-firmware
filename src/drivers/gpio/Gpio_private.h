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
#include <stdlib.h>

/* Internal includes */
#include "drivers/gpio/Gpio_public.h"
#include "drivers/gpio/Gpio_errors.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief Global instance of the GPIO driver state
 * 
 */
extern Gpio GPIO;

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/* Maximum number of programmable GPIOs set by TivaWare manual */
#define GPIO_MAX_NUM_GPIOS (43)

/* Number of GPIOs 
 * TODO: This is currently the same as the max number, but it is a separate
 * define just in case they end up being different later on. */
#define GPIO_NUM_GPIOS (43)

/* Maximum number of times to check that the GPIO peripheral is ready
 * TODO: This is arbitrary and based off of the current I2C peripheral
 * ready checks. */
#define GPIO_MAX_NUM_PERIPHERAL_READY_CHECKS (10)



extern Gpio_Module GPIO_PINS[GPIO_MAX_NUM_GPIOS];



/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

#endif /* H_GPIO_PRIVATE_H */