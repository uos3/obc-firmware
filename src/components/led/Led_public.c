/**
 * @ingroup component
 * @file Led_public.c
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
#include "util/debug/Debug_public.h"
#include "components/led/Led_public.h"
#include "components/led/Led_private.h"
#include "components/led/Led_errors.h"
#include "drivers/gpio/Gpio_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Led_set(uint8_t led_number_in, bool led_state_in) {
    /* Reference to the LED */
    Led_Module *p_led = &LED_LEDS[led_number_in];

    /* Raise an error if the maximum number of LEDs is exceeded */
    if (led_number_in >= LED_NUMBER_OF_LEDS) {
        DEBUG_ERR("LED input ID number greater than the number of LEDs");
        Debug_exit(1);
        return LED_ERROR_INVALID_LED_ID;
    }

    /* Write the desired state onto the specified led pin */
    if (Gpio_write(p_led->gpio_pin, led_state_in) != ERROR_NONE) {
        DEBUG_ERR("Failed to write to gpio pin");
        Debug_exit(1);
        return LED_ERROR_WRITE_FAILED;
    }

    /* Set the led state to the new state */
    p_led->state = led_state_in;

    return ERROR_NONE;
}

ErrorCode Led_toggle(uint8_t led_number_in) {
    /* Reference to the LED */
    Led_Module *p_led = &LED_LEDS[led_number_in];

    /* Raise an error if the maximum number of LEDs is exceeded */
    if (led_number_in >= LED_NUMBER_OF_LEDS) {
        DEBUG_ERR("LED input ID number greater than the number of LEDs");
        return LED_ERROR_INVALID_LED_ID;
    }

    /* Write the opposite state to what is currently on the LED */
    Gpio_write(p_led->gpio_pin, !p_led->state);

    /* Set the led state to the new state */
    p_led->state = !p_led->state;

    return ERROR_NONE;
}