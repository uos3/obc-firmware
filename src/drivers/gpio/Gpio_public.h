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
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "drivers/gpio/Gpio_public.h"
#include "drivers/gpio/Gpio_errors.h"


/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* Creates an index of pin (port and number) to be called in the functions */
typedef enum _GPIO_PIN_INDEX {
    GPIO_PINA0 = 0,
    GPIO_PINA1,
    GPIO_PINA2,
    GPIO_PINA3,
    GPIO_PINA4,
    GPIO_PINA5,
    GPIO_PINA6,
    GPIO_PINA7,
    GPIO_PINB0,
    GPIO_PINB1,
    GPIO_PINB2,
    GPIO_PINB3,
    GPIO_PINB4,
    GPIO_PINB5,
    GPIO_PINB6,
    GPIO_PINB7,
    GPIO_PINC0,
    GPIO_PINC1,
    GPIO_PINC2,
    GPIO_PINC3,
    GPIO_PINC4,
    GPIO_PINC5,
    GPIO_PINC6,
    GPIO_PINC7,
    GPIO_PIND0,
    GPIO_PIND1,
    GPIO_PIND2,
    GPIO_PIND3,
    GPIO_PIND4,
    GPIO_PIND5,
    GPIO_PIND6,
    GPIO_PIND7,
    GPIO_PINE0,
    GPIO_PINE1,
    GPIO_PINE2,
    GPIO_PINE3,
    GPIO_PINE4,
    GPIO_PINE5,
    GPIO_PINF0,
    GPIO_PINF1,
    GPIO_PINF2,
    GPIO_PINF3,
    GPIO_PINF4,
} ;

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Internal includes */
#include "drivers/gpio/Gpio_errors.h"

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

typedef enum _Gpio_Mode {

    GPIO_MODE_INPUT,

    GPIO_MODE_OUTPUT

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

#endif /* H_GPIO_PUBLIC_H */