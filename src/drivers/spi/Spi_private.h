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
#include "drivers/spi/Spi_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/* Number of SPI modules available in the TM4C */
#define NUM_OF_SPI_MODULES (4)
/* Number of enabled SPI modules, currently one is used for the radio (CC1125) and the other is for the FRAM */
#define NUM_SPI_MOD_ENABLED (2)
/* Maximum amount of attempts the software should attempt to start the SPI peripheral
NOTE: This number is abstract and doesn't depend on anything */
#define SPI_MAX_NUM_SPI_PERIPH_READY_CHECKS (10)
/* Maximum amount of attempts the software should attempt to start the GPIO peripheral
NOTE: This number is abstract and doesn't depend on anything */
#define SPI_MAX_NUM_GPIO_PERIPH_READY_CHECKS (10)

/* -------------------------------------------------------------------------   
 * STRUCTS/UNIONS
 * ------------------------------------------------------------------------- */

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
    uint8_t gpio_cs;
    uint32_t clockrate;
    bool initialised;
    bool wait_miso;
} Spi_Module;

typedef struct _Spi_Send_Single_Byte {
    /**
     * @brief This will send only a byte at a time and check for interrupts/events
     */
    uint8_t step;
    Spi_ActionStatus status;
    ErrorCode error;
    Spi_Device device;
    uint8_t byte;
} Spi_Send_Single_Byte;

typedef struct Spi {
    bool initialised;
    // TODO: Implement actions union, action types and module lock (Check if there is a module being used by another device)
} Spi;


/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief The global instance of the SPI driver state
 */
extern Spi SPI;

/**
 * @brief Creates an array of SPI modules available on the TM4C
 */
extern uint32_t SPI_MODULES[NUM_OF_SPI_MODULES];

/**
 * @brief Creates an array of enabled modules in the SPI
 */
extern uint32_t SPI_ENABLED_MODULES[NUM_SPI_MOD_ENABLED];

