#include "../delay.h"

#include <time.h>
#include <inttypes.h>
#include <asm/errno.h>

/** Public Functions */

void Delay_ms(uint32_t milliseconds)
{
  Delay_us(1000 * milliseconds);
}

void Delay_us(uint32_t microseconds)
{
  struct timespec req, rem;
  req.tv_sec = microseconds / (1000 * 1000);
  req.tv_nsec = (microseconds - (req.tv_sec*1000*1000))*1000;

  while(nanosleep(&req, &rem) == EINTR)
  {
    /* Interrupted by signal, shallow copy remaining time into request, and resume */
    req = rem;
  }
}