#include "../led.h"

#include <avr/io.h>

typedef struct LED {
  volatile uint8_t *peripheral_register;
  uint8_t peripheral;
  volatile uint8_t *port;
  uint8_t pin;
  bool initialised;
  enum { state_ON
       , state_OFF
       } state;
} LED;

static LED LED_leds[4] = 
  { { &DDRC, DDC0, &PORTC, PC0, false, state_OFF } // LED 1
  , { &DDRC, DDC1, &PORTC, PC1, false, state_OFF } // LED 2
  , { &DDRC, DDC2, &PORTC, PC2, false, state_OFF } // LED 3
  , { &DDRC, DDC3, &PORTC, PC3, false, state_OFF } // LED 4
  };
#define NUMBER_OF_LEDS  ( sizeof(LED_leds) / sizeof(LED) )

/* Generic Utility functions */
static void LED_init(LED *led)
{
  if(led->initialised)
    return;
  
  *led->peripheral_register |= _BV(led->peripheral);
}

void LED_on(uint8_t led_num)
{
  if(led_num >= NUMBER_OF_LEDS)
    return;

  LED_init(&LED_leds[led_num]);

  *LED_leds[led_num].port |= _BV(LED_leds[led_num].pin);

  LED_leds[led_num].state = state_ON;
}

void LED_off(uint8_t led_num)
{
  if(led_num >= NUMBER_OF_LEDS)
    return;
  
  LED_init(&LED_leds[led_num]);

  *LED_leds[led_num].port &= ~(_BV(LED_leds[led_num].pin));

  LED_leds[led_num].state = state_OFF;
}

void LED_set(uint8_t led_num, bool state)
{
  if(led_num >= NUMBER_OF_LEDS)
    return;
  
  if(state==true)
  {
    LED_on(led_num);
  }
  else
  {
    LED_off(led_num);
  }
}