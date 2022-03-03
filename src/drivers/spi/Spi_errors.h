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

#define SPI_ERROR_NOT_INITIALISED ((ErrorCode)MOD_ID_SPI | 1)

#define SPI_ERROR_SPI_PERIPH_ENABLE_FAILED ((ErrorCode)MOD_ID_SPI | 2)

#define SPI_ERROR_GPIO_PERIPH_ENABLE_FAILED ((ErrorCode)MOD_ID_SPI | 3)