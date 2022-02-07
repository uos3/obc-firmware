/**
 * @file Spi_private.h
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

typedef struct _Spi_Module {
    uint32_t peripheral_gpio;
    uint32_t peripheral_ssi;
    uint32_t base_gpio;
    uint32_t base_spi;
    uint32_t pin_ssi_clk_func;
    uint32_t pin_ssi_miso_func;
    uint32_t pin_ssi_mosi_func;
    uint8_t pin_ssi_miso;
    uint8_t pin_ssi_mosi;
    uint8_t pin_ssi_clk;
    uint32_t clockrate;
    bool initialised;
} Spi_Module;

typedef struct _Spi_Send {
    /* data */
} Spi_Send;

