/**
 * @file Spi_errors.h
 * @author Gianluca Borgo (gb2g20@soton.ac.uk/gianborgo@hotmail.com)
 * @brief Defines error codes for the SPI module.
 * 
 * 
 * 
 * @version 0.1
 * @date 2022-02-25
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The I2C driver has not been initialised.
 */
#define SPI_ERROR_NOT_INITIALISED ((ErrorCode)MOD_ID_SPI | 1)

/**
 * @brief Failed to enable an I2C peripheral.
 */
#define SPI_ERROR_SPI_PERIPH_ENABLE_FAILED ((ErrorCode)MOD_ID_SPI | 2)

/**
 * @brief Failed to enable a GPIO peripheral.
 */
#define SPI_ERROR_GPIO_PERIPH_ENABLE_FAILED ((ErrorCode)MOD_ID_SPI | 3)

/**
 * @brief Attempted to send 0 bytes.
 */
#define SPI_ERROR_ZERO_LENGTH_SEND ((ErrorCode)MOD_ID_SPI | 4)

/**
 * @brief An unexpected action step index was found while executing an
 * action.
 */
#define SPI_ERROR_UNEXPECTED_ACTION_STEP ((ErrorCode)MOD_ID_SPI | 5)

/**
 * @brief MISO failed to go to low on a given number of checks so the clock stability can't be checked
 */
#define SPI_ERROR_MISO_NOT_LOW ((ErrorCode)MOD_ID_SPI | 6)

/**
 * @brief Failed to flush FIFO.
 */
#define SPI_ERROR_FIFO_FLUSH_FAILED ((ErrorCode)MOD_ID_SPI | 7)

#define SPI_ERROR_MASTER_MODULE_BUSY ((ErrorCode)MOD_ID_SPI | 8)