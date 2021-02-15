/**
 * @file Wtd_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Watchdog Timer Module public header.
 * 
 * Task ref: [UT_2.8.5]
 * 
 * The Watchdog timer module provides a method of detecting system halts and
 * restarting the OBC. A system halt is when the normal cyclic execution of the
 * software stops, for example getting stuck in an infinite loop.
 * 
 * There are three watchdog sources on the UoS3 platform:
 *  - The TM4C's onboard watchdog timers (WDT_TM4C)
 *  - The watchdog chip on the TOBC board (WDT_TOBC) TODO: removed?
 *  - The EPS MCU (WDT_EPS)
 * 
 * TODO: Select which WDTs to support
 * 
 * @version 0.1
 * @date 2021-02-15
 * 
 * @copyright Copyright (c) UoS3 2021
 */

#ifndef H_WDT_PUBLIC_H
#define H_WTD_PUBLIC_H

#endif /* H_WTD_PUBLIC_H */