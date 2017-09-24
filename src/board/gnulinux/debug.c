/**
 * @ingroup gnulinux
 * @ingroup debug
 *
 * @file gnulinux/debug.c
 * @brief Debug message print - gnulinux platform
 *
 * @{
 */

#include "../debug.h"

#include <stdarg.h>
#include <stdio.h>

void Debug_print(char* fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  vprintf(fmt, arg);
  va_end(arg);
}

/**
 * @}
 */