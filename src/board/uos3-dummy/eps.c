/**
 * @ingroup uos3-proto
 * @ingroup eps
 *
 * @file eps.c
 *
 * @{
 */

#include "board.h"
#include "../uart.h"
#include "../rtc.h"
#include "../eps.h"

#define SW_ON   0x02
#define SW_OFF  0x33
#define DEBUG false

// DEBUG_NOTES ( REMOVE LATER )
#include "../../firmware.h"
#define UART_INTERFACE UART_GNSS

void EPS_init(void)
{

}

bool EPS_selfTest(void)
{
  return true;
}

bool EPS_getBatteryInfo(uint16_t *output, uint8_t type)
{
  *output = 0;
  return true;
}

bool EPS_setPowerRail(uint8_t type, bool on){
  return true;
}

/**
 * @}
 */
