#include "board.h"
#include "../gpio.h"

#include "../led.h"

/* LED description struct */
typedef struct LED {
  uint8_t gpio;
  bool state;
} LED;

/* Array of connected LEDs */
static LED LED_leds[2] = 
  { { GPIO_PC2, false } // LED A
  , { GPIO_PB1, false } // LED B
  };
#define NUMBER_OF_LEDS  ( sizeof(LED_leds) / sizeof(LED) )

/** Public Functions */

void LED_on(uint8_t led_num)
{
  LED_set(led_num, true);
}

void LED_off(uint8_t led_num)
{
  LED_set(led_num, false);
}

void LED_set(uint8_t led_num, bool state)
{
  if(led_num >= NUMBER_OF_LEDS)
    return;
  LED *led = &LED_leds[led_num];

  GPIO_write(led->gpio, state);

  led->state = state;
}

void LED_toggle(uint8_t led_num)
{ if(led_num >= NUMBER_OF_LEDS)
    return;
  LED *led = &LED_leds[led_num];
  GPIO_write(led->gpio, !led->state);
  led->state = !led->state;
}