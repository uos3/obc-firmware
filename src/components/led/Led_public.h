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
 * DEFINES
 * ------------------------------------------------------------------------- */

#define NUMBER_OF_LEDS (2)

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/* Struct defining an LED, with the pin and state */
typedef struct _Led_Module {
    uint8_t gpio_pin;
    bool state; /* Current state of the LED (true = on, false = off) */
} Led_Module;

/* TODO: Find more suitable name perhaps, look at software writing standards
 * document */
/* LEDs are connected to pins B1 and C2
 * TODO: find file location of schematic for reference */
static Led_Module LED_LEDS[NUMBER_OF_LEDS] = {
    {GPIO_PINB1, false},
    {GPIO_PINC2, false}
};

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