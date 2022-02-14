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

Spi_Module SPI_MODULES[] {
    {
    SYSCTL_PERIPH_GPIOF,
    SYSCTL_PERIPH_PERIPH_SSI1,
    GPIO_PORTF_BASE,
    SSI_BASE,
    GPIO_PF2_SSI1CLK,
    GPIO_PF0_SSI1RX,
    GPIO_PF1_SSI1TX,
    GPIO_PIN_2,
    GPIO_PING_0,
    GPIO_PIN_1,
    5000000,
    false
    }
}