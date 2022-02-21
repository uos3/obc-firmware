/**
 * @file Spi_public.h
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
#include <stdbool.h>
#include <stdint.h>

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

typedef enum _Spi_ActionStatus {
    SPI_ACTION_STATUS_NO_ACTION = 0,
    SPI_ACTION_STATUS_IN_PROGRESS,
    SPI_ACTION_STATUS_SUCCESS,
    SPI_ACTION_STATUS_FAILURE
} Spi_ActionStatus;

/* -------------------------------------------------------------------------   
 * STRUCTURES
 * ------------------------------------------------------------------------- */

typedef struct _Spi_Device {
    uint32_t module;
    uint8_t address;
} Spi_Device;
