/** @file blinky.c
 *
 * LED Blink Example Application
 *
 * @ingroup mains
 * @{
 */
#include "../mission/mission.h"

int main(void)
{
  Mission_init();
  while(1)
  {
    Mission_loop();
  }
}

/**
 * @}
 */
