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

#include "board.h"

#define DEBUG_A GPIO_PC4
#define DEBUG_B GPIO_PD4
#define DEBUG_C GPIO_PD5
#define DEBUG_D GPIO_PB0
#define DEBUG_E GPIO_PC5

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
