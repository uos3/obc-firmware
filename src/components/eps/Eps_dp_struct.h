/**
 * @file Eps_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief EPS Component Module DataPool structure definition.
 * 
 * Task ref: [UT_2.10.7]
 * 
 * @version 0.1
 * @date 2021-02-15
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_EPS_DP_STRUCT_H
#define H_EPS_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdlib.h>

/* Internal includes */
#include "system/kernel/Kernel_errors.h"
#include "components/eps/Eps_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

typedef struct _Eps_Dp {

    /**
     * @brief Flag indicating whether or not the Eps has been initialised.
     * 
     * @dp 1
     */
    bool INITIALISED;

    /**
     * @brief Stores errors that occur during operation.
     * 
     * @dp 2
     */
    ErrorCode ERROR_CODE;

} Eps_Dp;

#endif /* H_EPS_DP_STRUCT_H */