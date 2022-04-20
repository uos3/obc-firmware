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
#include "drivers/spi/Spi_errors.h"

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
uint32_t SPI_ENABLED_MODULES[NUM_SPI_MOD_ENABLED] = {0, 2};

ErrorCode Spi_action_single_send(Spi_ActionSingleSend *p_action_in) {

    /* Get the pointer to the SPI module this device is associated with */
    Spi_Module *p_spi_module = &SPI_MODULES[p_action_in->device.module];

    switch (p_action_in->step) {
    case 0:
        SSIDataPut(p_spi_module->base_spi, p_action_in->device.address);
        p_action_in->step++;

        __attribute__ ((fallthrough));
    
    case 1:
        /* TODO: Check for the wait_miso here create a variable that will check for an X amount of times (SPI_MAX_NUM_WAIT_MISO_CHECKS) */
    
    default:
        DEBUG_ERR("Reached unexpected step of single send action for the SPI driver");
        return SPI_ERROR_UNEXPECTED_ACTION_STEP;
    }

}