/**
 * @defgroup wdt
 *
 * @brief External watchdog driver
 *
 * @details Driver for configuring internal watchdog
 *
 * @ingroup drivers
 */

#ifndef __WDT_H__
#define __WDT_H__

#include "../firmware.h"
#include <stdint.h>
#include <stdbool.h>

uint32_t temporary_time2;             //used for controlling/reseting internal watchdogs
void setup_internal_watchdogs(void);  //setting up internal watchdogs-to be called in the beginning of mission init
void internal_wdt_handler(void);      //watchdogs int. handler
#endif