/** @file uos3-proto/random.c
 *
 * @ingroup random
 * @{
 */

 #include "board.h"

#include "../random.h"

#include <stdlib.h>
#include <stdbool.h>

static bool rng_seeded = false;

uint32_t Random(uint32_t max)
{
  uint32_t prev, out;
  
  if(!rng_seeded)
  {
     srand(4); // TODO: Use a better source
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