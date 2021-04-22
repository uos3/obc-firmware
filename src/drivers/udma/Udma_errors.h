/**
 * @ingroup udma
 * 
 * @file Gpio_public.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public header for the uDMA driver.
 * 
 * @version 0.1
 * @date 2020-12-06
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_UDMA_ERRORS_H
#define H_UDMA_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdint.h>

#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief TODO: Attempted to initialise the uDMA before the peripheral is
 * ready.
 */
#define UDMA_ERROR_PERIPHERAL_NOT_READY ((ErrorCode)MOD_ID_UDMA | 1)

/**
 * @brief TODO: Attempted to send or receive bytes while the uDMA has not yet
 * been initialised.
 */
#define UDMA_ERROR_NOT_INITIALISED ((ErrorCode)MOD_ID_UDMA | 2)

/**
 * @brief This ErrorCode is reserved for when uDMAErrorStatusGet returns a
 * non-zero error which does not match any other defined error.
 * 
 */
#define UDMA_ERROR_TRANSFER_FAILED ((ErrorCode)MOD_ID_UDMA | 3)

#endif /* H_UDMA_ERRORS_H */