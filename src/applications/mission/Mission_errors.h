/**
 * @file Mission_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Mission Application error code definitions
 * 
 * Task ref: [UT_2.11.1]
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_MISSION_ERRORS_H
#define H_MISSION_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_errors.h"

/* -------------------------------------------------------------------------   
 * ERRORS
 * ------------------------------------------------------------------------- */

/**
 * @brief A required dependent module (DataPool, EventManager, MemStoreManager)
 * is not initialised.
 */
#define MISSION_ERROR_DEPENDENCY_NOT_INIT ((ErrorCode)(MOD_ID_MISSION | 1))

/**
 * @brief The Mission module hasn't been initialised.
 */
#define MISSION_ERROR_NOT_INIT ((ErrorCode)(MOD_ID_MISSION | 2))

/**
 * @brief An error occured while using the EventManager. See
 * DP.EVENTMANAGER.ERROR_CODE for cause.
 */
#define MISSION_ERROR_EVENTMANAGER_ERROR ((ErrorCode)(MOD_ID_MISSION | 3))

#endif /* H_MISSION_ERRORS_H */