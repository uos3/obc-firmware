/* @file led.h
 * @ingroup drivers
 * @{
 */

#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * Switches on the specified LED peripheral
 *
 * @param led_number ID of the LED peripheral
 */
void LED_on(uint8_t led_number);

/**
 * Switches off the specified LED peripheral
 *
 * @param led_number ID of the LED peripheral
 */
void LED_off(uint8_t led_number);

/**
 * Sets the state of the specified LED peripheral
 *
 * @param led_number ID of the LED peripheral
 * @param state state to be set (true = on, false = off)
 */
void LED_set(uint8_t led_number, bool state);

#endif /*  __LED_H__ */

/**
 * @}
 */