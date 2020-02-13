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
 * Sets up a rising-edge interrupt on the specified GPIO pin
 *
 * @param gpio_number ID of the GPIO pin
 * @param pointer to the function to be called in the interrupt
 * @returns true on success, false on failure
 */
bool GPIO_set_risingInterrupt(uint8_t gpio_number, void (*interrupt_callback)(void));

/**
 * @function
 * @ingroup gpio
 *
 * Removes an interrupt on the specified GPIO pin
 *
 * @param gpio_number ID of the GPIO pin
 */
void GPIO_reset_interrupt(uint8_t gpio_number);

#endif /*  __GPIO_H__ */