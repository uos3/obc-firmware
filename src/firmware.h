/* This file contains all header files */

#ifndef __FIRMWARE_H__
#define __FIRMWARE_H__

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

/* Board-specific header file, completed by macro: 'board/<board>/board.h' */
#include BOARD_INCLUDE

/* Generic Board Module header files */
#include "board/board.h"
#include "board/led.h"
#include "board/uart.h"
#include "board/delay.h"
#include "board/wdt.h"
#include "board/spi.h"
#include "board/i2c.h"
#include "board/gpio.h"
#include "board/debug.h"
#include "board/random.h"
#include "board/ldpc.h"
/* Hardware Drivers */
#include "board/fram.h"
#include "board/temperature_sensors.h"
#include "board/watchdog_int.h"
#include "board/imu.h"

/* radio stuff */
#include "cc1125/cc112x_spi.h"
#include "cc1125/cc1125.h"

/* Packet */
#include "packet/packet.h"
#include "packet/shake.h"
#include "packet/crc16.h"
#include "packet/pn9.h"
#include "packet/interleave.h"
#include "packet/telemetry.h"

/* Buffer */
#include "buffer/buffer.h"

/* get Suzi's modules */

//#include "suz/i2cbase.h"
#include "suz/uartbase.h"

/* ... */

#endif /*  __FIRMWARE_H__ */
