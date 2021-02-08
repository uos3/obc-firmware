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

#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

#define NUMBER_OF_LEDS (2)

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

typedef struct _Led {
    uint8_t gpio_pin;
    bool state;
} Led;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/* TODO: For Led_set and Led_toggle, was initially a void function, but I
 * chose to make them a bool so they can return the state of the LED after
 * it has been altered (perhaps may be useful for debugging or testing), must
 * check if this is worth it, or if they should go back to void */
/**
 * @brief Sets the state of the LED peripheral
 * 
 * @param led_number_in ID number of the LED peripheral
 * @param led_state_in The state of which the LED is to be set to
 * @return Returns a bool of the state of the LED after setting
 */
bool Led_set(uint8_t led_number_in, bool led_state_in);

/**
 * @brief Toggles the state of the LED peripheral
 * 
 * @param led_number_in ID number of the LED peripheral
 * @return Returns a bool of the state of the LED after toggling
 */
bool Led_toggle(uint8_t led_number_in);

#endif /* H_LED_PUBLIC_H */