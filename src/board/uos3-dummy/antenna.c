/**
 * @ingroup uos3-proto
 * @ingroup antenna
 *
 * @file uos3-proto/antenna.c
 * @brief Antenna Hardware Driver - uos3-proto board
 *
 * @{
 */

#include "board.h"
#include "../gpio.h"
#include "../delay.h"

#include "../antenna.h"

void Antenna_deploy(void)
{
  GPIO_set(GPIO_PB5);
  Delay_ms(5000);
  GPIO_reset(GPIO_PB5);
}

bool Antenna_read_deployment_switch(void)
{
  return true;
}
