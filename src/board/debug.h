/**
 * @defgroup debug
 *
 * @brief Debug print Functions
 *
 * @details Prints debug messages.
 *
 * Only implemented on gnulinux so far
 *
 * @ingroup drivers
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

/**
 * @function
 * @ingroup debug
 *
 * Prints a format string with variadic arguments
 *
 * @param fmt printf-style format string
 */
void Debug_print(char* fmt, ...);

#endif /*  __DEBUG_H__ */