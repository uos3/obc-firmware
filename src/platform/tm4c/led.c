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

/* Driver Abstraction Macros */
#define PERIPH_init(periph) if(!SysCtlPeripheralReady(periph)) { \
                              SysCtlPeripheralEnable(periph);    \
                              while(!SysCtlPeripheralReady(periph)) {}; \
                            }
#define PIN_init(port, pin) GPIOPinTypeGPIOOutput(port, pin);
#define PIN_on(port, pin)  GPIOPinWrite(port, pin, pin);
#define PIN_off(port, pin) GPIOPinWrite(port, pin, 0);

/* Generic Utility functions */
static void LED_init(LED *led)
{
  if(led->initialised)
    return;
  PERIPH_init(led->peripheral);
  PIN_init(led->port, led->pin);
}

void LED_on(uint8_t led_num)
{
  LED_init(&LED_leds[led_num]);

  PIN_on(LED_leds[led_num].port, LED_leds[led_num].pin);
  LED_leds[led_num].state = state_ON;
}

void LED_off(uint8_t led_num)
{
  LED_init(&LED_leds[led_num]);

  PIN_off(LED_leds[led_num].port, LED_leds[led_num].pin);
  LED_leds[led_num].state = state_OFF;
}

void LED_set(uint8_t led_num, bool state)
{
  if(state==true)
  {
    LED_on(led_num);
  }
  else
  {
    LED_off(led_num);
  }
}