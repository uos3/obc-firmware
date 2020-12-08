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

/* TODO: Add board defines and constants for GPIO - most likely in board.h? */
/* TODO: Add gpio mode enum (or struct?) */
/* TODO: Check parts of prev. year's gpio.c which should be moved into gpio.h */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

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
    /* Prev year has void (*int_function)(void); in a different struct.
     * TODO: Check what its purpose is */
} Gpio_Module;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

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