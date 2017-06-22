/* This file contains all header files */

#ifndef __FIRMWARE_H__
#define __FIRMWARE_H__

#include <stdint.h>
#include <stdbool.h>

/* Board-specific header file, completed by macro: 'board/<board>/board.h' */
#include BOARD_INCLUDE

/* Generic Board Module header files */
#include "board/board.h"
#include "board/led.h"
#include "board/uart.h"

/* ... */

#endif /*  __FIRMWARE_H__ */