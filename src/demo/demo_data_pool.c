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
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int32_t main(void) {

    /* Init the debug system */
    Debug_init();

    DEBUG_INF("Initialising DataPool");

    /* Initialise the datapool */
    int32_t dp_init_ret = DataPool_init();
    DEBUG_INF("DataPool_init() = %d\n", dp_init_ret);

    /* Get a member of the data pool */
    bool dp_is_init = DP.INITIALISED;

    /* Get the same member by the ID (which we know should be 1) */
    bool *p_dp_is_init = NULL;
    DataPool_DataType data_type;
    size_t data_size;
    if (!DataPool_get(
        (DataPool_Id)0x0001,
        (void **)&p_dp_is_init,
        &data_type,
        &data_size
    )) {
        exit(1);
    }

    /* Print these out */
    DEBUG_INF("dp_is_init = %d", dp_is_init);
    DEBUG_INF("p_dp_is_init = %p", (void *)p_dp_is_init);
    DEBUG_INF("*p_dp_is_init = %d", *p_dp_is_init);
    DEBUG_INF(
        "data_type = %d (== DATAPOOL_DATATYPE_BOOL = %d)", 
        data_type,
        data_type == DATAPOOL_DATATYPE_BOOL
    );
    DEBUG_INF("data_size = %ld\n", data_size);

    /* Try with something that should be zero */
    bool *p_em_is_init = NULL;
    if (!DataPool_get(
        (DataPool_Id)0x0201,
        (void **)&p_em_is_init,
        &data_type,
        &data_size
    )) {
        exit(1);
    }
    DEBUG_INF("em_is_init = %d", DP.EVENTMANAGER.INITIALISED);
    DEBUG_INF("p_em_is_init = %p", (void *)p_em_is_init);
    DEBUG_INF("*p_em_is_init = %d", *p_em_is_init);
    DEBUG_INF(
        "data_type = %d (== DATAPOOL_DATATYPE_BOOL = %d)", 
        data_type,
        data_type == DATAPOOL_DATATYPE_BOOL
    );
    DEBUG_INF("data_size = %ld\n", data_size);

    /* Print out some DP names */
    char *p_symbol;
    if (!DataPool_get_symbol_str(
        (DataPool_Id)0x0001,
        &p_symbol
    )) {
        exit(1);
    }
    DEBUG_INF("%s", p_symbol);

    free(p_symbol);

    if (!DataPool_get_symbol_str(
        (DataPool_Id)0x0201,
        &p_symbol
    )) {
        exit(1);
    }
    DEBUG_INF("%s", p_symbol);

    free(p_symbol);

    /* Access an invalid DP id */
    DEBUG_INF("Trying invalid DP ID");
    DataPool_get((DataPool_Id)0x0000, NULL, NULL, NULL);

    return 0;
}

/** @} */ /* End of demo_launchpad_blinky */