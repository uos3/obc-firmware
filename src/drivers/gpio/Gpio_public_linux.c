/**
 * @ingroup gpio
 * 
 * @file Gpio_public_linux.c
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public GPIO driver for the firmware.
 * 
 * Implements the GPIO driver for Linux.
 * 
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard Lirary */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Internal */
#include "drivers/gpio/Gpio_public.h"
#include "drivers/gpio/Gpio_private.h"
#include "drivers/gpio/Gpio_errors.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */