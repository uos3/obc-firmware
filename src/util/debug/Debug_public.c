/**
 * @ingroup debug
 * 
 * @file Debug_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides debug logging functionality to the software.
 * @version 0.1
 * @date 2020-11-01
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef TARGET_UNIX
#include <time.h>
#endif

/* Internal includes */
#include "util/debug/Debug_public.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

#ifdef TARGET_UNIX
static struct timespec DEBUG_INIT_TIME;
#endif

/* -------------------------------------------------------------------------   
 * CONSTANTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Names given to different log levels.
 */
static const char *Debug_level_names[] = {
  "ERR", "WRN", "---", "DBG", "TRC"
};

/**
 * @brief Terminal colour codes used to log messages
 */
static const char *Debug_level_colours[] = {
   "\x1b[31m", "\x1b[33m", "", "\x1b[36m", "\x1b[35m",
};

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Debug_init(void) {

    /* If on UNIX set the init time */
    #ifdef TARGET_UNIX
    clock_gettime(CLOCK_MONOTONIC_RAW, &DEBUG_INIT_TIME);
    #endif

    return true;
}

#ifdef TARGET_UNIX
void Debug_log_unix(
    uint8_t level, 
    const char *p_file, 
    uint32_t line, 
    const char *p_fmt, 
    ...
) {
    va_list args;
    va_start(args, p_fmt);

    /* Calculate time since init */
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    int64_t delta_ms = 
        (now.tv_sec - DEBUG_INIT_TIME.tv_sec) * 1000
        + (now.tv_nsec - DEBUG_INIT_TIME.tv_nsec) / 1000000;

    /* Remove the file path up to src/ */
    char *p_file_stripped = strstr(p_file, "src");

    /* Print the prefix */
    printf(
        "[%10lu %s%s\x1b[0m] %s:%d ",
        delta_ms,
        Debug_level_colours[level], 
        Debug_level_names[level],
        p_file_stripped,
        line
    );

    /* Print the message */
    vprintf(p_fmt, args);

    /* Print newline */
    printf("\n");
}
#endif

#ifdef TARGET_TM4C
void Debug_log_tm4c(
    uint8_t level, 
    const char *p_file, 
    uint32_t line, 
    const char *p_fmt, 
    ...
) {
    /* TODO */
}
#endif

void Debug_exit(int error_code) {
    #ifdef TARGET_UNIX
    exit(error_code);
    #endif
    #ifdef TARGET_TM4C
    __asm("BKPT");
    #endif
}