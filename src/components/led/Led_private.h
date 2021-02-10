/**
 * @ingroup component
 * @file Led_private.h
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

#ifndef H_LED_PRIVATE_H
#define H_LED_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "drivers/gpio/Gpio_public.h"
#include "drivers/board/Board_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/* Define the constant number of LEDs occupying pins (currently 2 from TOBC
 * diagram, see sharepoint) */
#define LED_NUMBER_OF_LEDS (2)

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/* Struct defining an LED, with the pin and state */
typedef struct _Led_Module {
    uint8_t gpio_pin;
    bool state; /* Current state of the LED (true = on, false = off) */
} Led_Module;

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief Global instance of the LED component state.
 * 
 */
extern Led_Module LED_LEDS;

#endif /* H_LED_PRIVATE_H */