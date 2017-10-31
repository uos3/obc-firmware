/**
 * @defgroup led
 *
 * @brief LED Peripheral Driver
 *
 * @details Hardware LED Peripheral Driver.
 *
 * LEDs for each board are defined in ${board}/board.h
 *
 * @ingroup drivers
 */

#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @function
 * @ingroup led
 *
 * Switches on the specified LED peripheral
 *
 * @param led_number ID of the LED peripheral
 */
void LED_on(uint8_t led_number);

/**
 * @function
 * @ingroup led
 *
 * Switches off the specified LED peripheral
 *
 * @param led_number ID of the LED peripheral
 */
void LED_off(uint8_t led_number);

/**
 * @function
 * @ingroup led
 *
 * Sets the state of the specified LED peripheral
 *
 * @param led_number ID of the LED peripheral
 * @param state state to be set (true = on, false = off)
 */
void LED_set(uint8_t led_number, bool state);

void LED_toggle(uint8_t led_num);

#endif /*  __LED_H__ */
