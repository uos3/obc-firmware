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
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/* TODO: Are the two functions below (gpio_set and gpio_reset) reduntand due
 * to gpio_write, as you can write either high (set) or low (reset) within? */
/**
 * @brief Sets the state of a specified GPIO pin to HIGH.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_set(uint8_t gpio_id_number);

/**
 * @brief Sets the state of a specified GPIO pin to LOW.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_reset(uint8_t gpio_id_number);

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
 * @return ErrorCode Return
 */
ErrorCode Gpio_read(uint8_t gpio_id_number, bool *p_gpio_value_out);

/* TODO: Come back to these on 2020-12-07
 * ErrorCode Gpio_set_rising_interrupt();
 * ErrorCode Gpio_reset_interrupt();*/

#endif /* H_GPIO_PUBLIC_H */