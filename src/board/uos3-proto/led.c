#include "../led.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/* LED description struct */
typedef struct LED {
  uint32_t peripheral;  // TI Driver Peripheral Reference
  uint32_t port;        // TI Driver Port Reference
  uint8_t  pin;         // TI Driver Pin Reference
  bool initialised;
  bool state;
} LED;

/* Array of connected LEDs */
static LED LED_leds[2] = 
  { { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_2, false, false } // LED A
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_1, false, false } // LED B
  };
#define NUMBER_OF_LEDS  ( sizeof(LED_leds) / sizeof(LED) )

/* Initialise Hardware for an LED */
static void LED_init(LED *led)
{
  /* Check LED is initialised, exit if it is */
  if(led->initialised)
    return;

  /* Initialise Peripheral if not already initialised */
  if(!SysCtlPeripheralReady(led->peripheral))
  {
    SysCtlPeripheralEnable(led->peripheral);
    while(!SysCtlPeripheralReady(led->peripheral)) { };
  }

  /* Initialise LED Pin on Port */
  GPIOPinTypeGPIOOutput(led->port, led->pin);

  led->initialised = true;
}

/** Public Functions */

void LED_on(uint8_t led_num)
{
  LED_set(led_num, true);
}

void LED_off(uint8_t led_num)
{
  LED_set(led_num, false);
}

/* Abstract for weird TI third argument */
#define LED_Pin_Write(led, pin_value)  GPIOPinWrite(led->port, led->pin, pin_value)

void LED_set(uint8_t led_num, bool state)
{
  if(led_num >= NUMBER_OF_LEDS)
    return;
  LED *led = &LED_leds[led_num];

  LED_init(led);

  /* Write value to LED pin, abstracted to macro above */
  /* - second argument is for LOW: 0, for HIGH: LED_leds[led_num].pin */
  if(state == true)
  {
    LED_Pin_Write(led, led->pin);
  }
  else
  {
    LED_Pin_Write(led, 0);
  }

  led->state = state;
}