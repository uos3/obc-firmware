/**
 * @ingroup uos3-proto
 * @ingroup gpio
 *
 * @file uos3-proto/gpio.c
 * @brief GPIO Driver - uos3-proto board
 *
 * @{
 */

#include "board.h"
#include "../gpio.h"

#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include <stddef.h>

/* LED description struct */
typedef struct GPIO {
  uint32_t peripheral;  // TI Driver Peripheral Reference
  uint32_t port;        // TI Driver Port Reference
  uint8_t  pin;         // TI Driver Pin Reference
  uint8_t  int_pin;     // TI Driver Pin Interrupt Reference
  bool initialised;
  GPIO_mode mode;
  void (*int_function)(void);
} GPIO;

static GPIO GPIO_gpios[48] = 
  { { SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_INT_PIN_0, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_INT_PIN_1, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_INT_PIN_2, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_INT_PIN_3, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_INT_PIN_4, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_5, GPIO_INT_PIN_5, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_6, GPIO_INT_PIN_6, false, GPIO_MODE_INPUT, NULL }
  , { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_7, GPIO_INT_PIN_7, false, GPIO_MODE_INPUT, NULL }
  };
#define NUMBER_OF_GPIOS  ( sizeof(GPIO_gpios) / sizeof(GPIO) )

static void GPIO_mode_init(GPIO *gpio, GPIO_mode mode)
{
  switch(mode)
  {
    case GPIO_MODE_INPUT:
      GPIOPinTypeGPIOInput(gpio->port, gpio->pin);
      break;

    case GPIO_MODE_OUTPUT:
      GPIOPinTypeGPIOOutput(gpio->port, gpio->pin);
      break;

    default:
      break;
  }

  gpio->mode = mode;
}

static void GPIO_init(GPIO *gpio, GPIO_mode mode)
{
  if(!gpio->initialised)
  {
    /* Initialise Peripheral if not already initialised */
    if(!SysCtlPeripheralReady(gpio->peripheral))
    {
      SysCtlPeripheralEnable(gpio->peripheral);
      while(!SysCtlPeripheralReady(gpio->peripheral)) { };
    }

    GPIO_mode_init(gpio, mode);

    gpio->initialised = true;
  }
  else if(mode != gpio->mode)
  {
    GPIO_mode_init(gpio, mode);
  }
}

void GPIO_set(uint8_t gpio_num)
{
  GPIO_write(gpio_num, true);
}

void GPIO_reset(uint8_t gpio_num)
{
  GPIO_write(gpio_num, false);
}

/* Third argument is a bit mask of bits to be set */
#define GPIO_Pin_Write(gpio, value)  GPIOPinWrite(gpio->port, gpio->pin, value ? gpio->pin : 0x00)

void GPIO_write(uint8_t gpio_num, bool state)
{
  if(gpio_num >= NUMBER_OF_GPIOS)
    return;
  GPIO *gpio = &GPIO_gpios[gpio_num];

  GPIO_init(gpio, GPIO_MODE_OUTPUT);

  GPIO_Pin_Write(gpio, state);
}

/* Bit mask the returned 32 bits for the required pin and convert to boolean */
#define GPIO_Pin_Read(gpio)  !!(GPIOPinRead(gpio->port, gpio->pin) & gpio->pin)

bool GPIO_read(uint8_t gpio_num)
{
  if(gpio_num >= NUMBER_OF_GPIOS)
    return false;
  GPIO *gpio = &GPIO_gpios[gpio_num];

  GPIO_init(gpio, GPIO_MODE_INPUT);

  return GPIO_Pin_Read(gpio);
}

static inline void GPIO_InterruptHandler_GPIO(uint32_t _status, uint8_t _gpio_number)
{
  if((_status & GPIO_gpios[_gpio_number].int_pin) && (GPIO_gpios[_gpio_number].int_function != NULL))
  {
    GPIO_gpios[_gpio_number].int_function();
  }
}

static void GPIO_PortA_InterruptHandler(void)
{
  uint32_t status;

  /* Retrieve GPIO Port Interrupt Status word */
  status = GPIOIntStatus(GPIO_PORTA_BASE,true);

  if(status)
  {
    /* Clear GPIO Port Interrupt Status */
    GPIOIntClear(GPIO_PORTA_BASE, status);

    /* Check each GPIO */
    GPIO_InterruptHandler_GPIO(status, GPIO_PA0);
    GPIO_InterruptHandler_GPIO(status, GPIO_PA1);
    GPIO_InterruptHandler_GPIO(status, GPIO_PA2);
    GPIO_InterruptHandler_GPIO(status, GPIO_PA3);
    GPIO_InterruptHandler_GPIO(status, GPIO_PA4);
    GPIO_InterruptHandler_GPIO(status, GPIO_PA5);
    GPIO_InterruptHandler_GPIO(status, GPIO_PA6);
    GPIO_InterruptHandler_GPIO(status, GPIO_PA7);
  }
}

bool GPIO_set_risingInterrupt(uint8_t gpio_number, void interrupt_callback(void))
{
  if(gpio_number >= NUMBER_OF_GPIOS)
    return false;
  GPIO *gpio = &GPIO_gpios[gpio_number];

  GPIO_init(gpio, GPIO_MODE_INPUT);

  GPIOIntTypeSet(gpio->port, gpio->pin, GPIO_RISING_EDGE);

  gpio->int_function = interrupt_callback;

  if(gpio->port == GPIO_PORTA_BASE)
  {
    GPIOIntRegister(gpio->port, GPIO_PortA_InterruptHandler);
  }
  else
  {
    return false;
  }
  // TODO - Implement other ports

  GPIOIntEnable(gpio->port, gpio->int_pin);

  return true;
}

void GPIO_reset_interrupt(uint8_t gpio_number)
{
  if(gpio_number >= NUMBER_OF_GPIOS)
    return;
  GPIO *gpio = &GPIO_gpios[gpio_number];

  GPIOIntDisable(gpio->port, gpio->int_pin);

  GPIOIntUnregister(gpio->port);

  gpio->int_function = NULL;
}

/**
 * @}
 */