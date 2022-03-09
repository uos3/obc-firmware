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

/**
 * @brief Maximum number of actions that can occur in a cycle for the SPI driver
 * 
 * This values comes from the number of the components using SPI, in this case:
 *  - Radio
 *  - FRAM
 */
#define SPI_MAX_NUM_ACTIONS (2)
/**
 * @brief Number of SPI modules available in the TM4C
 */
#define NUM_OF_SPI_MODULES (4)

/**
 * @brief Number of enabled SPI modules, currently one is used for the radio (CC1125) and the other is for the FRAM
 */
#define NUM_SPI_MOD_ENABLED (2)

/**
 * @brief Maximum amount of attempts the software should attempt to start the SPI peripheral
 * NOTE: This number is abstract and doesn't depend on anything
 */
#define SPI_MAX_NUM_SPI_PERIPH_READY_CHECKS (10)

/**
 * @brief Maximum amount of attempts the software should attempt to start the GPIO peripheral
 * NOTE: This number is abstract and doesn't depend on anything 
 */
#define SPI_MAX_NUM_GPIO_PERIPH_READY_CHECKS (10)

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

typedef enum _Spi_ActionType {
    SPI_ACTION_TYPE_NONE,
    SPI_ACTION_TYPE_SINGLE_SEND,
    SPI_ACTION_TYPE_SINGLE_RECV,
    SPI_ACTION_TYPE_BURST_SEND,
    SPI_ACTION_TYPE_BURST_RECV
} Spi_ActionType;

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

typedef struct _Spi_Send_Single_Send {
    /**
     * @brief This will send only a byte at a time and check for interrupts/events
     * 
     * The action is broken down into a number of steps that execute sequentially. At each step transition the action can exit while it waits
     * for some event (i.e. an interrupt) to occur.
     */
    uint8_t step;

    /**
     * @brief The current status of this action.
     */
    Spi_ActionStatus status;

    /**
     * @brief Stores any error associated with this action.
     */
    ErrorCode error;

    /**
     * @brief The device to perform this action on
     */
    Spi_Device device;

    /**
     * @brief The byte to send to the device
     */
    uint8_t byte;
    
} Spi_ActionSingleSend;

typedef union _Spi_Action {
    Spi_ActionSingleSend single_send;
} Spi_Action;

typedef struct Spi {
    /**
     * @brief This is a flag that will indicate weather the driver has been initialised
     */
    bool initialised;
    // TODO: Implement actions union, action types and module lock (Check if there is a module being used by another device)
    /**
     * @brief Array of action types associated with each element in u_action
     */
    Spi_ActionType action_types[SPI_MAX_NUM_ACTIONS];
    /**
     * @brief Array of actions to be performed
     */
    Spi_Action u_actions[SPI_MAX_NUM_ACTIONS];
    /**
     * @brief True if the indexed module is locked by a device
     */
    bool module_locked[NUM_OF_SPI_MODULES];
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

