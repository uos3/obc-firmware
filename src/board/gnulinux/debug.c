#include "../debug.h"

#include <stdarg.h>
#include <stdio.h>

void Debug_print(char* fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  vprintf(fmt, arg);
  va_end(arg);
}
