/**
 * @ingroup gnulinux
 * @ingroup led
 *
 * @file gnulinux/led.c
 * @brief LED Driver - gnulinux platform
 *
 * @{
 */

#include "../led.h"

/** Public Functions */

void LED_on(uint8_t led_num)
{
  (void) led_num;
}

void LED_off(uint8_t led_num)
{
  (void) led_num;
}

void LED_set(uint8_t led_num, bool state)
{
  (void) led_num;
  (void) state;
}

/**
 * @}
 */