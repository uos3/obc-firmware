/**
 * @ingroup drivers
 * 
 * @file Board_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Main driver providing basic TM4C123GPMH board support.
 * 
 * Task ref: [UT_2.8.1]
 * 
 * This driver is responsible for setting up the system clock on
 * initialisation.
 * 
 * @version 0.1
 * @date 2020-11-11
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup board Board
 * @{
 */

#ifndef H_BOARD_PUBLIC_H
#define H_BOARD_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* None */

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Number of checks to do of SysCtlPeripheralReady before an error
 * should be returned.
 */
#define BOARD_NUM_PERIPH_ENABLED_CHECKS (10)

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the board driver by configuring the system clock.
 */
bool Board_init(void);


#endif /* H_BOARD_PUBLIC_H */

/** @} */ /* End of Board group */