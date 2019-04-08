/**
 * @defgroup board
 *
 * @brief Board Initialisation Functions
 *
 * @ingroup drivers
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "../firmware.h"

/**
 * @function
 * @ingroup board
 *
 * Initialises the board/platform.
 *
 */
void Board_init(void);

void WTD_wait_to_kick();

#endif /*  __BOARD_H__ */