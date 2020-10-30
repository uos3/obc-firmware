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
#include "system/data_pool/DataPool_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int32_t main(void) {

    printf("Initialising DataPool\n");

    /* Initialise the datapool */
    int32_t dp_init_ret = DataPool_init();
    printf("DataPool_init() = %d\n", dp_init_ret);

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
    printf("dp_is_init = %d\n", dp_is_init);
    printf("p_dp_is_init = %p\n", p_dp_is_init);
    printf("*p_dp_is_init = %d\n", *p_dp_is_init);
    printf(
        "data_type = %d (== DATAPOOL_DATATYPE_BOOL = %d)\n", 
        data_type,
        data_type == DATAPOOL_DATATYPE_BOOL
    );
    printf("data_size = %ld\n", data_size);

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
    printf("em_is_init = %d\n", DP.EVENTMANAGER.INITIALISED);
    printf("p_em_is_init = %p\n", p_em_is_init);
    printf("*p_em_is_init = %d\n", *p_em_is_init);
    printf(
        "data_type = %d (== DATAPOOL_DATATYPE_BOOL = %d)\n", 
        data_type,
        data_type == DATAPOOL_DATATYPE_BOOL
    );
    printf("data_size = %ld\n", data_size);

    /* Finally print out some DP names */
    char *p_symbol;
    if (!DataPool_get_symbol_str(
        (DataPool_Id)0x0001,
        &p_symbol
    )) {
        exit(1);
    }
    printf("%s\n", p_symbol);

    free(p_symbol);

    if (!DataPool_get_symbol_str(
        (DataPool_Id)0x0201,
        &p_symbol
    )) {
        exit(1);
    }
    printf("%s\n", p_symbol);

    free(p_symbol);

    return 0;
}

/** @} */ /* End of demo_launchpad_blinky */