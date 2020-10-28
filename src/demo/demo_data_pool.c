/**
 * @ingroup demo
 * 
 * @file demo_data_pool.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Short demo of using the DataPool
 * @version 0.1
 * @date 2020-10-21
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup demo_data_pool Demo Data Pool
 * @{
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdio.h>

/* Internal includes */
#include "system/data_pool/DataPool_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int32_t main(void) {

    printf("Initialising DataPool\n");

    /* Initialise the datapool */
    int32_t dp_init_ret = DataPool_init();
    printf("DataPool_init() = %d\n", dp_init_ret);

    /* Get value of the init key */
    printf("DP_U8_INITIALISED = %d\n", DP.INITIALISED);

    return 0;
}

/** @} */ /* End of demo_launchpad_blinky */