/**
 * @ingroup event_manager
 * 
 * @file EventManager_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides DataPool parameters for the EventManager
 * @version 0.1
 * @date 2020-10-30
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_EVENTMANAGER_DP_STRUCT_H
#define H_EVENTMANAGER_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdbool.h>

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

typedef struct _EventManager_Dp {

    /**
     * @brief Indicates whether or not the EventManager has been initialised.
     * 
     * @dp 1
     */
    bool INITIALISED;
} EventManager_Dp;

#endif /* H_EVENTMANAGER_DP_STRUCT_H */