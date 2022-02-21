/**
 * @file Spi_public_tm4c.c
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Internal Includes */
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "drivers/spi/Spi_private.h"
#include "drivers/spi/Spi_public.h"

/* External Includes */
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Spi_init() {
    for (size_t i = 0; i < NUM_SPI_MOD_ENABLED; i++) {
        /* Getting a reference to the module to init */
        Spi_Module *p_module = &SPI_MODULES[SPI_ENABLED_MODULES[i]];

        /* Checking if the module has already been initialised */
        if (p_module->initialised) {
            DEBUG_WRN("SPI module %d has already been initialised and is being called again on Spi_init()", SPI_ENABLED_MODULES[i]);

            /* Move to the next module on the list */
            continue;
        }

    }
    
}