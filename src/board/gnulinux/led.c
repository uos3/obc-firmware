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
#include <stdio.h>

/** Public Functions */

void LED_on(uint8_t led_num)
{
  // change status

  // print status of all LEDs

  // old code (should be conv. to above)
  printf("[LED%d ON]\n", led_num);
  fflush(stdout);
}

void LED_off(uint8_t led_num)
{
  printf("[LED%d OFF]\n", led_num);
  fflush(stdout);
}

void LED_set(uint8_t led_num, bool state)
{
  if (state == 1)
    LED_on(led_num);
  else
    LED_off(led_num);
}

/**
 * @}
 */
