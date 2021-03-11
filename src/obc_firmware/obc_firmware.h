/**
 * @file obc_firmware.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Generic local function definitions for main firmware entry point.
 * 
 * @version 0.1
 * @date 2021-03-11
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_OBC_FIRMWARE_H
#define H_OBC_FIRMWARE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Perform the early module init, which includes:
 *  - Critical system modules, as defined by the Kernel
 *  - All drivers
 */
void obc_firmware_init_early_modules(void);

/**
 * @brief Initialise all remaining system modules
 */
void obc_firmware_init_system(void);

/**
 * @brief Initialise all components.
 */
void obc_firmware_init_components(void);

/**
 * @brief Initialise all applications
 */
void obc_firmware_init_applications(void);

/**
 * @brief Step the drivers
 */
void obc_firmware_step_drivers(void);

/**
 * @brief Step the components
 */
void obc_firmware_step_components(void);

/**
 * @brief Step the system modules.
 * 
 * Note that this includes applications through the use of the OpModeManager.
 */
void obc_firmware_step_system(void);

#endif /* H_OBC_FIRMWARE_H */