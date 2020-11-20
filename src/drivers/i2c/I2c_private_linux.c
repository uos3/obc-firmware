/**
 * @ingroup i2c
 * 
 * @file I2c_private_linux.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Private source for the I2C driver.
 * 
 * This file implements the I2C driver for Linux targets.
 * 
 * @version 0.1
 * @date 2020-11-12
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "drivers/i2c/I2c_private.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief The global instance of the I2C driver state.
 */
I2c I2C;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool I2c_devices_equal(I2c_Device *p_a_in, I2c_Device *p_b_in) {
    return (p_a_in->module == p_b_in->module) 
        && 
        (p_a_in->address == p_b_in->address);
}