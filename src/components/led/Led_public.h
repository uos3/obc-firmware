/**
 * @ingroup component
 * @file Led_public.h
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

#ifndef H_LED_PUBLIC_H
#define H_LED_PUBLIC_H

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
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Sets the state of the LED peripheral
 * 
 * @param led_number_in ID number of the LED peripheral
 * @param led_state_in The state of which the LED is to be set to
 * @return Returns a bool of the state of the LED after setting
 */
ErrorCode Led_set(uint8_t led_number_in, bool led_state_in);

/**
 * @brief Toggles the state of the LED peripheral
 * 
 * @param led_number_in ID number of the LED peripheral
 * @return Returns a bool of the state of the LED after toggling
 */
ErrorCode Led_toggle(uint8_t led_number_in);

#endif /* H_LED_PUBLIC_H */