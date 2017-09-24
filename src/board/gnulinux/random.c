/** @file random.c
 *
 * Random driver.
 *
 * @author Phil Crump
 *
 * @ingroup drivers
 * @{
 */

#include "board.h"

#include "../random.h"

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

static bool rng_seeded = false;

uint32_t Random(uint32_t max)
{
  uint32_t prev, out;
  
  if(!rng_seeded)
  {
     srand((unsigned int)time(NULL));
     rng_seeded = true;
  }

  out = (uint32_t)rand();
  prev = out;

  while(prev == out)
  {
    out = (uint32_t)rand();
  }

  return (uint32_t)((float)out * ((float)max / (float)RAND_MAX));
}

/**
 * @}
 */