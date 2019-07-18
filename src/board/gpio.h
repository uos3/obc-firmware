/**
 * @defgroup gpio
 *
 * @brief GPIO Peripheral Driver
 *
 * @details Hardware GPIO Peripheral Driver.
 *
 * GPIO pins for each board are defined in ${board}/board.h
 *
 * @ingroup drivers
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT} GPIO_mode;

/**
 * @function
 * @ingroup gpio
 *
 * Sets the state (High) on the specified GPIO pin
 *
 * @param gpio_number ID of the GPIO pin
 */
void GPIO_set(uint8_t gpio_number);

/**
 * @function
 * @ingroup gpio
 *
 * Resets the state (Low) on the specified GPIO pin
 *
 * @param gpio_number ID of the GPIO pin
 */
void GPIO_reset(uint8_t gpio_number);

/**
 * @function
 * @ingroup gpio
 *
 * Writes a given state on the specified GPIO pin
 *
 * @param gpio_number ID of the GPIO pin
 * @param state state to be written (true = High, false = Low)
 */
void GPIO_write(uint8_t gpio_number, bool state);

/**
 * @function
 * @ingroup gpio
 *
 * Reads the input value on the specified GPIO pin
 *
 * @param gpio_number ID of the GPIO pin
 * @returns input value of the specified GPIO pin (true = High, false = Low)
 */
bool GPIO_read(uint8_t gpio_number);

/**
 * @function
 * @ingroup gpio
 *
 * initialise and enable the interrupt on selected gpio input pin
 * second function is for clearing this interrupt
 *
 * @param gpio_number ID of the GPIO pin
 * @param interrupt_type type of the interrupt to be selected from the int_types[] table - gpio.c file
 * @param void (*intHandler)(void) function to be called when the interrupt occurs
 */
void gpio_interrupt_enable(uint8_t gpio_num, uint8_t interrupt_type, void (*intHandler)(void));
void gpio_interrupt_clear(uint8_t gpio_num, uint8_t interrupt_type);

#endif /*  __GPIO_H__ */