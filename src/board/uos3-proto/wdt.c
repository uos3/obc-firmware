#include "board.h"
#include "../gpio.h"
#include "../delay.h"

#include "../wdt.h"

/* WDT description struct */
typedef struct WDT {
  uint8_t gpio;
} WDT;

static WDT wdt = { GPIO_PF4 };

/** Public Functions */

void WDT_kick(void)
{
  /* 10uS Pulse (WDT wants minimum of 3uS) */
  GPIO_set(wdt.gpio);
  Delay_us(10);
  GPIO_reset(wdt.gpio);
}