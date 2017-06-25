#include "../wdt.h"
#include "../delay.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/* WDT description struct */
typedef struct WDT {
  uint32_t peripheral;  // TI Driver Peripheral Reference
  uint32_t port;        // TI Driver Port Reference
  uint8_t  pin;         // TI Driver Pin Reference
  bool initialised;
} WDT;

static WDT wdt = { SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_4, false};

/* Abstract for weird TI third argument */
#define WDT_Pin_Write(wdt, pin_value)  GPIOPinWrite((wdt)->port, (wdt)->pin, pin_value)

/* Initialise Hardware for interfacing with the Watchdog Timer */
static void WDT_init(WDT *wdt)
{
  /* Check LED is initialised, exit if it is */
  if(wdt->initialised)
    return;

  /* Initialise Peripheral if not already initialised */
  if(!SysCtlPeripheralReady(wdt->peripheral))
  {
    SysCtlPeripheralEnable(wdt->peripheral);
    while(!SysCtlPeripheralReady(wdt->peripheral)) { };
  }

  /* Initialise LED Pin on Port */
  GPIOPinTypeGPIOOutput(wdt->port, wdt->pin);
  WDT_Pin_Write(wdt, 0);

  wdt->initialised = true;
}

/** Public Functions */

void WDT_kick(void)
{
  WDT_init(&wdt);

  WDT_Pin_Write(&wdt, wdt.pin);
  Delay_us(10);
  WDT_Pin_Write(&wdt, 0);
}