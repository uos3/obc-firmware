/**
 * @ingroup i2c
 * 
 * @file I2c_private_tm4c.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Private source for the I2C driver.
 * 
 * This file implements the I2C driver for TM4C targets.
 * 
 * @version 0.1
 * @date 2020-11-11
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* External includes */
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"

#include "inc/hw_memmap.h"

/* Internal includes */
#include "drivers/i2c/I2c_private.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

I2c I2C;

I2c_Module I2C_MODULES[I2C_NUM_MODULES] = {
    /* I2C module 0 */
    {
        SYSCTL_PERIPH_GPIOB,
        SYSCTL_PERIPH_I2C0,
        GPIO_PORTB_BASE,
        I2C0_BASE,
        GPIO_PB2_I2C0SCL,
        GPIO_PB3_I2C0SDA,
        GPIO_PIN_2,
        GPIO_PIN_3,
        false,
        false
    },

    /* I2C module 1 */
    {
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_I2C1,
        GPIO_PORTA_BASE,
        I2C1_BASE,
        GPIO_PA6_I2C1SCL,
        GPIO_PA7_I2C1SDA,
        GPIO_PIN_6,
        GPIO_PIN_7,
        false,
        false
    },

    /* I2C module 2 */
    {
        SYSCTL_PERIPH_GPIOE,
        SYSCTL_PERIPH_I2C2,
        GPIO_PORTE_BASE,
        I2C2_BASE,
        GPIO_PE4_I2C2SCL,
        GPIO_PE5_I2C2SDA,
        GPIO_PIN_4,
        GPIO_PIN_5,
        false,
        false
    },

    /* I2C module 3 */
    {
        SYSCTL_PERIPH_GPIOD,
        SYSCTL_PERIPH_I2C3,
        GPIO_PORTD_BASE,
        I2C3_BASE,
        GPIO_PD0_I2C3SCL,
        GPIO_PD1_I2C3SDA,
        GPIO_PIN_0,
        GPIO_PIN_1,
        false,
        false
    }
};