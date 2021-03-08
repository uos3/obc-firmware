/**
 * @file OpModeManager_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief OpModeManager datapool structure definition.
 * 
 * Task ref: [UT_2.9.10]
 * 
 * @version 0.1
 * @date 22021-03-08
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_OPMODEMANAGER_DP_STRUCT_H
#define H_OPMODEMANAGER_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES 
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_module_ids.h"
#include "system/opmode_manager/OpModeManager_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief OpModeManager Application DataPool structure.
 */
typedef struct _OpModeManager_Dp {
    
    /**
     * @brief Flag indicating if the OpModeManager App has been initialised (true) or
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
    OpModeManager_OpMode OPMODE;

    /**
     * @brief The next OpMode that will be set when the current mode change is
     * complete. 
     * 
     * If there is no mode change happening this value shall be the same as
     * DP.OPMODEMANAGER.OPMODE.
     * 
     * @dp 4
     */
    OpModeManager_OpMode NEXT_OPMODE;

    /**
     * @brief Indicates whether or not a change in the OPMODE is in progress.
     * 
     * @dp 5
     */
    bool OPMODE_CHANGE_IN_PROGRESS;

} OpModeManager_Dp;

#endif /* H_OPMODEMANAGER_DP_STRUCT_H */