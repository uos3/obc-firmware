/**
 * @ingroup drivers
 * 
 * @file Board_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Main driver providing basic TM4C123GPMH board support.
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
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the board driver by configuring the system clock.
 */
bool Board_init(void);


#endif /* H_BOARD_PUBLIC_H */

/** @} */ /* End of Board group */