/**
 * @ingroup uos3-proto
 * @ingroup delay
 *
 * @file uos3-proto/delay.c
 * @brief Time Delay Driver - uos3-proto board
 *
 * @{
 */

#include "delay.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"

/** Public Functions */

void Delay_ms(uint32_t milliseconds)
{
  Delay_us(1000 * milliseconds);
}

void Delay_us(uint32_t microseconds)
{
  SysCtlDelay(13 * microseconds);    // empirical
}

/**
 * @}
 */