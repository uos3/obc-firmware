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
        /* Attempt to flush FIFO */
        bool flushed_fifo = false;
        for (int i = 0; i < SPI_MAX_NUM_ATTEMPT_FLUSH_FIFO; i++) {
            if (SSIDataGetNonBlocking(p_spi_module->base_spi, p_action_in->fifo_byte)) {
                flushed_fifo = true;
                break;
            }
        }

        if (!flushed_fifo) {
            DEBUG_ERR("Could not flush FIFO for SPI module %d", p_action_in->device.module);
            p_action_in->status = SPI_ACTION_STATUS_FAILURE;
            p_action_in->error = SPI_ERROR_FIFO_FLUSH_FAILED;
            return SPI_ERROR_FIFO_FLUSH_FAILED;
        }

        /* TODO: Check for the wait_miso here create a variable that will check for an X amount of times (SPI_MAX_NUM_WAIT_MISO_CHECKS) PARTIALLY COMPLETED */
        /* It will only wait for MISO it is configured to do so in the module */
        if (p_spi_module->wait_miso) {
            bool waiting_miso = true;
            for (int i = 0; i < SPI_MAX_NUM_CHECKS_WAIT_MISO; i++) {
                if (GPIOPinRead(p_spi_module->base_gpio, p_spi_module->pin_ssi_miso)) {
                    waiting_miso = false;
                    break;
                }
            }

            if (waiting_miso) {
                DEBUG_ERR("SPI module %d has attempted to wait for MISO, this means \
                MISO isn't low and that means the clock stability of the cc1120 can't be indicated.", p_action_in->device.module);
                p_action_in->status = SPI_ACTION_STATUS_FAILURE;
                p_action_in->error = SPI_ERROR_MISO_NOT_LOW;
                return SPI_ERROR_MISO_NOT_LOW;
            }
        }

        /* Increment the step index */
        p_action_in->step++;

        /* 
        * This GCC attribute prevents the fallthrough warning for this
        * case. Usually fallthrough in switches is bad, but here we
        * actually want this so that we do not interrupt the function when
        * it has the possibility to actually make progress. Therefore this
        * attribute is used to keep the fallthrough warning enabled but to
        * say that here it is ok
        */
        __attribute__ ((fallthrough));
    
    case 1:
        /* Write to the address and read the status byte */
        SSIDataPut(p_spi_module->base_spi, p_action_in->device.address);
        SSIDataGet(p_spi_module->base_spi, p_action_in->status_byte);

        /* Write data */
        if (p_action_in->byte != NULL) {
            SSIDataPut(p_spi_module->base_spi, p_action_in->byte);
            SSIDataGet(p_spi_module->base_spi, p_action_in->fifo_byte);
        }
        
        /* Increment the step index */
        p_action_in->step++;

        __attribute__ ((fallthrough));

    case 2:
        if (p_action_in->num_master_busy_checks >= SPI_MAX_NUM_MASTER_MAJOR_BUSY_CHECKS) {
                DEBUG_ERR(
                "SPI master module %d has been busy for %d major loops, now\
                considered unresponsive.",
                p_action_in->device.module,
                p_action_in->num_master_busy_major_checks
                );
                p_action_in->status = SPI_ACTION_STATUS_FAILURE;
                p_action_in->error = SPI_ERROR_MASTER_MODULE_BUSY;
                return SPI_ERROR_MASTER_MODULE_BUSY;
        }

        bool master_busy = true;
        for (int i = 0; i < SPI_MAX_NUM_MASTER_MINOR_BUSY_CHECKS; i++) {
            master_busy = SSIBusy(p_spi_module->base_spi);

            if (!master_busy) {
                break;
            }
            
        }

        if (master_busy) {
            p_action_in->num_master_busy_checks++;

            DEBUG_DBG("I2C master module %d still busy after minor check.",
            p_action_in->device.module);

            return ERROR_NONE;
        }

        /* Increment the step index */
        p_action_in->step++;

        __attribute__ ((fallthrough));

    case 3:
        return ERROR_NONE;
        
    
    default:
        DEBUG_ERR("Reached unexpected step of single send action for the SPI driver");
        return SPI_ERROR_UNEXPECTED_ACTION_STEP;
    }

}