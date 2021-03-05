/**
 * @file Mission_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Mission Application datapool structure definition.
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_MISSION_DP_STRUCT_H
#define H_MISSION_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES 
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_module_ids.h"
#include "applications/mission/Mission_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Mission Application DataPool structure.
 */
typedef struct _Mission_Dp {
    
    /**
     * @brief Flag indicating if the Mission App has been initialised (true) or
     * not (false).
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

    /**
     * @brief The current mission Operational Mode (OPMODE)
     * 
     * @dp 3
     */
    Mission_OpMode OPMODE;

    /**
     * @brief The next OpMode that will be set when the current mode change is
     * complete. 
     * 
     * If there is no mode change happening this value shall be the same as
     * DP.MISSION.OPMODE.
     * 
     * @dp 4
     */
    Mission_OpMode NEXT_OPMODE;

    /**
     * @brief Indicates whether or not a change in the OPMODE is in progress.
     * 
     * @dp 5
     */
    bool OPMODE_CHANGE_IN_PROGRESS;

} Mission_Dp;

#endif /* H_MISSION_DP_STRUCT_H */