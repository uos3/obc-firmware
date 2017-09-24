/* This file contains all header files */

#ifndef __FIRMWARE_H__
#define __FIRMWARE_H__

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Board-specific header file, completed by macro: 'board/<board>/board.h' */
#include BOARD_INCLUDE

/* Generic Board Module header files */
#include "board/board.h"
#include "board/led.h"
#include "board/uart.h"
#include "board/delay.h"
#include "board/wdt.h"
#include "board/spi.h"
#include "board/gpio.h"
#include "board/debug.h"
#include "board/random.h"

/* radio stuff */
#include "cc1125/cc112x_spi.h"
#include "cc1125/cc1125.h"

/* Packet */
#include "packet/sha3.h"
#include "packet/crc16.h"
#include "packet/pn9.h"
#include "packet/interleave.h"

/* get Suzi's modules */

#include "suz/i2cbase.h"
#include "suz/uartbase.h"

/* ... */

#endif /*  __FIRMWARE_H__ */

/*! \mainpage UoS³ Flight Computer Firmware Documentation
 *
 * The firmware is built from a top-level C program file and resolves all the references and includes from the src/ and specified hardware folders
 *
 * The design is such so that the main hardware code and hardware can be easily varied. eg camera code which can be used with different boards goes in the src folder, but the actual board specific drivers go in the relevant board folder.
 *
 * For installation instructions, see README.md in the Repository
 *
 * \section install_sec Related Links
 *
 * <a href="https://github.com/uos3/firmware">Github Repository</a>
 *
 * <a href="https://travis-ci.org/uos3/firmware">Travis CI</a>
 */