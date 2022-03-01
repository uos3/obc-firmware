/**
 * @file Spi_private_tm4c.c
 * @author Gianluca Borgo (gb2g20@soton.ac.uk/gianborgo@hotmail.com)
 * @brief This is the private SPI driver for the cubesat's firmware.
 * 
 * 
 * 
 * @version 0.1
 * @date 2021-10-22
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal Includes */
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/spi/Spi_private.h"

/* External Includes */
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

Spi_Module SPI_MODULES[NUM_OF_SPI_MODULES] = {
    // TODO: Add the pins for the other 3 modules
    {
        SYSCTL_PERIPH_GPIOF,
        SYSCTL_PERIPH_SSI1,
        GPIO_PORTF_BASE,
        SSI1_BASE,
        GPIO_PF2_SSI1CLK,
        GPIO_PF0_SSI1RX,
        GPIO_PF1_SSI1TX,
        GPIO_PIN_2,
        GPIO_PIN_0,
        GPIO_PIN_1,
        GPIO_PA3,
        5000000,
        false,
        true
    },

    {
        SYSCTL_PERIPH_GPIOG,
        SYSCTL_PERIPH_SSI2,
        GPIO_PORTG_BASE,
        SSI2_BASE,
        GPIO_PB4_SSI2CLK,
        GPIO_PB6_SSI2RX,
        GPIO_PB7_SSI2TX,
        GPIO_PIN_3,
        GPIO_PIN_4,
        GPIO_PIN_5,



    }
};

/* TODO: For now I put a dummy array similar to the I2C, these are the only 2 modules that are currently using SPI, there still isn't anything for index 2. This will be added later
 * when I implement the pins for the other 3 modules mentioned above, this means the 0 and 2 indexes will certainly change. */
uint32_t SPI_ENABLED_MODULES[NUM_SPI_MOD_ENABLED] = {0, 2}