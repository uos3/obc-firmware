/**
 * @file OpModeManager_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief OpModeManager error code definitions.
 * 
 * Task ref: [UT_2.9.10]
 * 
 * @version 0.1
 * @date 2021-03-08
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_OPMODEMANAGER_ERRORS_H
#define H_OPMODEMANAGER_ERRORS_H

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
#define OPMODEMANAGER_ERROR_DEPENDENCY_NOT_INIT ((ErrorCode)(MOD_ID_OPMODEMANAGER | 1))

/**
 * @brief The OpModeManager module hasn't been initialised.
 */
#define OPMODEMANAGER_ERROR_NOT_INIT ((ErrorCode)(MOD_ID_OPMODEMANAGER | 2))

/**
 * @brief An error occured while using the EventManager. See
 * DP.EVENTMANAGER.ERROR_CODE for cause.
 */
#define OPMODEMANAGER_ERROR_EVENTMANAGER_ERROR ((ErrorCode)(MOD_ID_OPMODEMANAGER | 3))

/**
 * @brief The value of DP.OPMODEMANAGER.STATE is invalid.
 */
#define OPMODEMANAGER_ERROR_INVALID_STATE ((ErrorCode)(MOD_ID_OPMODEMANAGER | 4))

#endif /* H_OPMODEMANAGER_ERRORS_H */