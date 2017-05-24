#include "../led.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

typedef struct LED {
  uint32_t peripheral;
  uint32_t port;
  uint8_t  pin;
  bool initialised;
  enum { state_ON
       , state_OFF
       } state;
} LED;

static LED LED_leds[2] = 
  { { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_2, false, state_OFF } // LED A
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_1, false, state_OFF } // LED B
  };
#define NUMBER_OF_LEDS  ( sizeof(LED_leds) / sizeof(LED) )

/* Driver Abstraction Macros */
#define PERIPH_init(led) if(!SysCtlPeripheralReady((led)->peripheral)) { \
                              SysCtlPeripheralEnable((led)->peripheral);    \
                              while(!SysCtlPeripheralReady((led)->peripheral)) {}; \
                            }
#define PIN_init(led) GPIOPinTypeGPIOOutput((led)->port, (led)->pin);
#define PIN_on(led)   GPIOPinWrite((led)->port, (led)->pin, (led)->pin);
#define PIN_off(led)  GPIOPinWrite((led)->port, (led)->pin, 0);

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