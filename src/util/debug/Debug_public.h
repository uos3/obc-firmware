/**
 * @ingroup util
 * 
 * @file Debug_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides debug logging functionality to the software.
 * 
 * Task ref: [UT_2.12.1]
 * 
 * @version 0.1
 * @date 2020-11-01
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup debug Debug Module
 * @{
 */

#ifndef H_DEBUG_PUBLIC_H
#define H_DEBUG_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief ARM stack context state frame.
 * 
 * See https://interrupt.memfault.com/blog/cortex-m-fault-debug.
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) _Debug_ContextStateFrame {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t return_address;
  uint32_t xpsr;
} Debug_ContextStateFrame;


/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/**
 * @brief The level of logging warnings to use.
 */
typedef enum _Debug_Level {
    DEBUG_LEVEL_ERROR = 0,
    DEBUG_LEVEL_WARNING = 1,
    DEBUG_LEVEL_INFO = 2,
    DEBUG_LEVEL_DEBUG = 3,
    DEBUG_LEVEL_TRACE = 4
} Debug_Level;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the debug module.
 * 
 * @return true Debug initialisation succeeded.
 * @return false Failed
 */
bool Debug_init(void);

#ifdef TARGET_UNIX
/**
 * @brief Log a message on unix to stdout.
 * 
 * NOTE: Use the provided debug macros (DEBUG_INF for example) rather than this
 * function. 
 * 
 * @param level The level to log the information at
 * @param p_file The file the log occurs on
 * @param line The line numbe of the log
 * @param p_fmt The format string of the log
 * @param ... The variadic formats for p_fmt.
 */
void Debug_log_unix(
    uint8_t level, 
    const char *p_file, 
    uint32_t line, 
    const char *p_fmt, 
    ...
);
#endif

#ifdef TARGET_TM4C
/**
 * @brief Log a message on the TM4C to UART TBD.
 * 
 * This function logs to UART1, which is the UART device connected to the
 * breakout pins on the TM4C launchpad. This should be changed to the correct
 * debug UART on the TOBC.
 * 
 * NOTE: Use the provided debug macros (DEBUG_INF for example) rather than this
 * function. 
 * 
 * @param level The level to log the information at
 * @param p_file The file the log occurs on
 * @param line The line numbe of the log
 * @param p_fmt The format string of the log
 * @param ... The variadic formats for p_fmt.
 */
void Debug_log_tm4c(
    uint8_t level, 
    const char *p_file, 
    uint32_t line, 
    const char *p_fmt, 
    ...
);
#endif

/**
 * @brief Exit execution.
 * 
 * Under Unix this will call `exit()`, however under the TM4C this will put a 
 * breakpoint.
 * 
 * @param error_code The code to exit with.
 */
void Debug_exit(int error_code);

/* -------------------------------------------------------------------------   
 * MACROS
 * ------------------------------------------------------------------------- */

#ifdef DEBUG_MODE
#ifdef TARGET_TM4C
#define _DEBUG_LOG(level, file, line, ...) Debug_log_tm4c(\
    level,\
    file,\
    line,\
    __VA_ARGS__)
#endif
#ifdef TARGET_UNIX
#define _DEBUG_LOG(level, file, line, ...) Debug_log_unix(\
    level,\
    file,\
    line,\
    __VA_ARGS__)
#endif
#else
#define _DEBUG_LOG(level, file, line, ...)
#endif

/**
 * @brief Print an error message to the Debug system.
 */
#define DEBUG_ERR(...) _DEBUG_LOG(\
    DEBUG_LEVEL_ERROR,\
    __FILE__,\
    __LINE__,\
    __VA_ARGS__\
)

/**
 * @brief Print an info message to the Debug system.
 */
#define DEBUG_WRN(...) _DEBUG_LOG(\
    DEBUG_LEVEL_WARNING,\
    __FILE__,\
    __LINE__,\
    __VA_ARGS__\
)

/**
 * @brief Print an info message to the Debug system.
 */
#define DEBUG_INF(...) _DEBUG_LOG(\
    DEBUG_LEVEL_INFO,\
    __FILE__,\
    __LINE__,\
    __VA_ARGS__\
)

/**
 * @brief Print a debug message to the Debug system.
 */
#define DEBUG_DBG(...) _DEBUG_LOG(\
    DEBUG_LEVEL_DEBUG,\
    __FILE__,\
    __LINE__,\
    __VA_ARGS__\
)


/**
 * @brief Print a trace message to the Debug system.
 */
#define DEBUG_TRC(...) _DEBUG_LOG(\
    DEBUG_LEVEL_TRACE,\
    __FILE__,\
    __LINE__,\
    __VA_ARGS__\
)


#endif /* H_DEBUG_PUBLIC_H */

/** @} */ /* End of event_manager */