/**
 * @ingroup gpio
 * 
 * @file Gpio_public.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public header for the GPIO driver.
 * 
 * @version 0.1
 * @date 2020-12-06
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_GPIO_PUBLIC_H
#define H_GPIO_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "drivers/gpio/Gpio_errors.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* Creates an index of pin (port and number) to be called in the functions */
typedef enum _GPIO_PIN_INDEX {
    GPIO_PINA0 = 0,
    GPIO_PINA1 = 1,
    GPIO_PINA2 = 2,
    GPIO_PINA3 = 3,
    GPIO_PINA4 = 4,
    GPIO_PINA5 = 5,
    GPIO_PINA6 = 6,
    GPIO_PINA7 = 7,
    GPIO_PINB0 = 8,
    GPIO_PINB1 = 9,
    GPIO_PINB2 = 10,
    GPIO_PINB3 = 11,
    GPIO_PINB4 = 12,
    GPIO_PINB5 = 13,
    GPIO_PINB6 = 14,
    GPIO_PINB7 = 15,
    GPIO_PINC0 = 16,
    GPIO_PINC1 = 17,
    GPIO_PINC2 = 18,
    GPIO_PINC3 = 19,
    GPIO_PINC4 = 20,
    GPIO_PINC5 = 21,
    GPIO_PINC6 = 22,
    GPIO_PINC7 = 23,
    GPIO_PIND0 = 24,
    GPIO_PIND1 = 25,
    GPIO_PIND2 = 26,
    GPIO_PIND3 = 27,
    GPIO_PIND4 = 28,
    GPIO_PIND5 = 29,
    GPIO_PIND6 = 30,
    GPIO_PIND7 = 31,
    GPIO_PINE0 = 32,
    GPIO_PINE1 = 33,
    GPIO_PINE2 = 34,
    GPIO_PINE3 = 35,
    GPIO_PINE4 = 36,
    GPIO_PINE5 = 37,
    GPIO_PINF0 = 38,
    GPIO_PINF1 = 39,
    GPIO_PINF2 = 40,
    GPIO_PINF3 = 41,
    GPIO_PINF4 = 42,
} GPIO_PIN_INDEX;

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

typedef enum _Gpio_Mode {

    GPIO_MODE_INPUT = 0,

    GPIO_MODE_OUTPUT = 1,

} Gpio_Mode;

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Defines a GPIO module
 * 
 */
typedef struct _Gpio_Module {
    uint32_t peripheral;
    uint32_t port;
    uint8_t pin;
    uint8_t interrupt_pin;
    Gpio_Mode mode;
    bool initialised;
    void (*int_function)(void); /* Interrupt function */
} Gpio_Module;

/**
 * @brief State of the GPIO driver
 * TODO: Does this have to be separate to GPIO module? I have created it as
 * a seperate struct so there can be a global state, so other modules can check
 * if it has been (for example) initialised.
 * 
 */
typedef struct _Gpio {
    
    /**
     * @brief Flag to show if the Gpio has been initalised or not
     * 
     */
    bool initialised;

} Gpio;

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

/**
 * @brief Writes the state of a specified GPIO pin to HIGH or LOW.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @param gpio_state State to write the pin to (true = HIGH, false = LOW)
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_write(uint8_t gpio_id_number, bool gpio_state);

/**
 * @brief Reads the input value of a specified GPIO pin.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @param p_gpio_value_out Input value of GPIO pin (true = HIGH, false = LOW).
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_read(uint8_t gpio_id_number, bool *p_gpio_value_out);

/**
 * @brief Sets up a rising edge interrupt on a specified GPIO pin.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @param interrupt_callback Pointer to the function which is called in the interrupt.
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_set_rising_interrupt(uint8_t gpio_id_number, void *interrupt_callback(void));

/**
 * @brief Removes the interrupt from a specified GPIO pin.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_reset_interrupt(uint8_t gpio_id_number);

/**
 * @brief Handles the interrupt, looping through all pins between the lower
 * and upper limits (as arguments).
 * 
 * @param gpio_int_status_in 
 * @param gpio_pin_lower_in 
 * @param gpio_pin_upper_in 
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_handle_interrupt(uint32_t gpio_int_status_in, uint8_t gpio_pin_lower_in, uint8_t gpio_pin_upper_in);

#endif /* H_GPIO_PUBLIC_H */