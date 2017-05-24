#include "../led.h"

#include <avr/io.h>

typedef struct LED {
  uint8_t *peripheral_register;
  uint8_t peripheral;
  uint8_t *port;
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

/* Driver Abstraction Macros */
#define PERIPH_init(led) *(led)->peripheral_register |= _BV((led)->peripheral);
#define PIN_init(led) 
#define PIN_on(led)      *(led)->port |= _BV((led)->pin);
#define PIN_off(led)     *(led)->port &= ~(_BV((led)->pin));

/* Generic Utility functions */
static void LED_init(LED *led)
{
  if(led->initialised)
    return;
  PERIPH_init(led);
  PIN_init(led);
}

void LED_on(uint8_t led_num)
{
  if(led_num >= NUMBER_OF_LEDS)
    return;

  LED_init(&LED_leds[led_num]);

  PIN_on(&LED_leds[led_num]);
  LED_leds[led_num].state = state_ON;
}

void LED_off(uint8_t led_num)
{
  if(led_num >= NUMBER_OF_LEDS)
    return;
  
  LED_init(&LED_leds[led_num]);

  PIN_off(&LED_leds[led_num]);
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