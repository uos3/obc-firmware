/**
 * @ingroup data_pool
 * 
 * @file DataPool_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides DataPool management and access functionality.
 * 
 * See corresponding header file for more information.
 * 
 * @version 0.1
 * @date 2020-10-21
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdbool.h>
#include <string.h>

/* Internal includes */
#include "system/data_pool/DataPool_public.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

DataPool DP;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool DataPool_init(void) {

    /* Zero the DataPool */
    memset(&DP, 0, sizeof(DP));

    /* Set the DP initialised flag */
    DP.INITIALISED = true;

    return true;
}
