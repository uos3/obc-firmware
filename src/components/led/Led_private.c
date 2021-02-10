/**
 * @ingroup component
 * @file Led_private.c
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief LED component module.
 * 
 * TODO:
 * Task ref: []
 * 
 * This module provides an interface for the onboard LED (mostly for testing
 * and debugging purposes)
 * 
 * @version 0.1
 * @date 2021-02-07
 * 
 * @copyright Copyright (c) 2021
 * 
 * @defgroup led LED
 * @{
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "components/led/Led_public.h"
#include "components/led/Led_private.h"
#include "components/led/Led_errors.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* LEDs are connected to pins B1 and C2 (see "TOBC Diagram" on sharepoint) */
Led_Module LED_LEDS[LED_NUMBER_OF_LEDS] = {
    {GPIO_PINB1, false},
    {GPIO_PINC2, false}
};