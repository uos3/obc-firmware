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
#include "drivers/spi/Spi_errors.h"

/* External Includes */
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Spi_init(void) {
    for (size_t i = 0; i < NUM_SPI_MOD_ENABLED; i++) {
        /* Getting a reference to the module to init */
        Spi_Module *p_module = &SPI_MODULES[SPI_ENABLED_MODULES[i]];

        /* Checking if the module has already been initialised */
        if (p_module->initialised) {
            DEBUG_WRN("SPI module %d has already been initialised and is being called again on Spi_init()", SPI_ENABLED_MODULES[i]);

            /* Move to the next module on the list */
            continue;
        }

        /* Reset peripheral if it is not ready */
        if (!SysCtlPeripheralReady(p_module->peripheral_ssi)) {
            SysCtlPeripheralReset(p_module->peripheral_ssi);
            SysCtlPeripheralSleepEnable(p_module->peripheral_ssi);

            /* Attempt to initialise the peripheral */
            bool enabled = false;
            for (int attempt = 0; attempt < SPI_MAX_NUM_SPI_PERIPH_READY_CHECKS; ++attempt) {
                if (SysCtlPeripheralReady(p_module->peripheral_ssi)) {
                    enabled = true;
                    break;
                }
            }

            /* If the peripheral wasn't enabled before the max attempts was reached, return an error */
            if (!enabled) {
                DEBUG_ERR("Could not enable SPI module %d", SPI_ENABLED_MODULES[i]);
                return SPI_ERROR_SPI_PERIPH_ENABLE_FAILED;
            }
        }

        /* Reset the peripheral if it is not ready */
        if (!SysCtlPeripheralReady(p_module->peripheral_gpio)) {
            SysCtlPeripheralReset(p_module->peripheral_gpio);
            SysCtlPeripheralEnable(p_module->peripheral_gpio);

            bool enabled = false;
            for (int attempt = 0; attempt < SPI_MAX_NUM_GPIO_PERIPH_READY_CHECKS; ++attempt) {
                if (SysCtlPeripheralReady(p_module->peripheral_gpio)) {
                    enabled = true;
                    break;
                }
            }

            /* If the peripheral wasn't enabled before the max attempts was reached, return an error */
            if (!enabled) {
                DEBUG_ERR("Could not enable SPI module %d", SPI_ENABLED_MODULES[i]);
                return SPI_ERROR_GPIO_PERIPH_ENABLE_FAILED;
            }

        }
        
        /* Configure the GPIO pins for their SPI functions */
        GPIOPinConfigure(p_module->pin_ssi_clk_func);
        GPIOPinConfigure(p_module->pin_ssi_miso_func);
        GPIOPinConfigure(p_module->pin_ssi_mosi_func);

        /* Assigning pins to the SPI peripheral */
        GPIOPinTypeSSI(p_module->base_gpio, p_module->pin_ssi_clk | p_module->pin_ssi_miso | p_module->pin_ssi_mosi);
        SSIClockSourceSet(p_module->base_spi, SSI_CLOCK_SYSTEM);
        
        /* Set the SPI clock rate */
        SSIConfigSetExpClk(p_module->base_spi, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, p_module->clockrate, 8);

        /* Enable the SPI master mode */
        SSIEnable(p_module->base_spi);

        p_module->initialised = true;
    }
    /* Make sure the SPI driver is marked as initialised */
    SPI.initialised = true;

    /* Return initialisation success */
    return ERROR_NONE;
}

ErrorCode Spi_step(void) {
    
}

ErrorCode Spi_device_send_bytes(Spi_Device *p_Device_in, uint8_t *p_data_in, size_t length_in) {
    /* Checking if the SPI module is initialised */
    if (!SPI.initialised) {
        DEBUG_ERR("Attempted to send byte(s) to SPI when it wasn't initialised.");
        return SPI_ERROR_NOT_INITIALISED;
    }

    /* TODO: Check if the module can be locked */

    /* Check if the length of the data going in is 0, we can't send 0 bytes */
    if (length_in == 0) {
        DEBUG_ERR("Attempted to send zero bytes.");
        return SPI_ERROR_ZERO_LENGTH_SEND;
    }
    
    
}