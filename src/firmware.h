/* This file contains all header files */

#ifndef __FIRMWARE_H__
#define __FIRMWARE_H__

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>
#include <limits.h>

/* Round up N to the next multiple of S */
#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

/* Mark binary constants as known GCC extension to suppress warning */
#define GCC_BINARY(x)	(__extension__ x)

/* Board-specific header file, completed by macro: 'board/<board>/board.h' */
#include BOARD_INCLUDE

/* Utilities */
#include "utility/circ_queue.h"
#include "utility/crc.h"
#include "utility/pn9.h"
#include "utility/shake.h"
/* Generic Board Module header files */
#include "board/antenna.h"
#include "board/board.h"
#include "board/led.h"
#include "board/uart.h"
#include "board/delay.h"
#include "board/wdt.h"
#include "board/spi.h"
#include "board/rtc.h"
#include "board/eeprom.h"
#include "board/gpio.h"
#include "board/debug.h"
#include "board/random.h"
#include "board/ldpc.h"
#include "board/memory_map.h"
#include "board/i2c.h"
/* Hardware Drivers */
#include "board/fram.h"
#include "board/temperature_sensors.h"
#include "board/watchdog_ext.h"
#include "board/imu.h"
#include "board/camera.h"
#include "board/eps.h"
#include "board/gnss.h"
#include "board/uos3-proto/board.h"

/* Mission */
//#include "mission/mission.h"

/* radio stuff */
#include "board/radio.h"
#include "cc112x/cc112x_spi.h"
#include "cc112x/cc112x.h"

/* Packet */
#include "packet/packet.h"
#include "packet/interleave.h"
#include "packet/telemetry.h"
#include "packet/cw.h"

/* Buffer */
#include "buffer/buffer.h"

/* Configuration */
#include "configuration/configuration.h"

/* Mission */
#include "mission/mission.h"

/* Test Interface */

#define UART_INTERFACE UART_DEBUG_4

/* ... */

#endif /*  __FIRMWARE_H__ */