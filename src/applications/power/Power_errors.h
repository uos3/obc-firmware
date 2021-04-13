 /**
 * @file Power_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application error code definitions
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_POWER_ERRORS_H
#define H_POWER_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_errors.h"
#include "system/kernel/Kernel_module_ids.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief A required dependent module (DataPool, EventManager, MemStoreManager,
 * EPS) was not initialised.
 */
#define POWER_ERROR_DEPENDENCY_NOT_INIT ((ErrorCode)(MOD_ID_POWER | 1))

/**
 * @brief The app's primary task timer couldn't be started due to a timer
 * error. See DP.POWER.TIMER_ERROR_CODE for the root cause.
 */
#define POWER_ERROR_TASK_TIMER_NOT_STARTED ((ErrorCode)(MOD_ID_POWER | 2))

/**
 * @brief Couldn't disable the timer associated with the primary task. See
 * DP.POWER.TIMER_ERROR_CODE for root cause.
 */
#define POWER_ERROR_TASK_TIMER_NOT_DISABLED ((ErrorCode)(MOD_ID_POWER | 3))

/**
 * @brief Indicates an error with the event manager occured. Check
 * DP.EVENT_MANAGER.ERROR_CODE for root cause.
 */
#define POWER_ERROR_EVENTMANAGER_ERROR ((ErrorCode)(MOD_ID_POWER | 4))

/**
 * @brief A call to Eps_collect_hk_data() failed. See DP.EPS.ERROR_CODE for the
 * error cause.
 */
#define POWER_ERROR_EPS_COLLECT_HK_FAILED ((ErrorCode)(MOD_ID_POWER | 5))

/**
 * @brief A call to Eps_send_config() failed. See DP.EPS.ERROR_CODE for the
 * error cause.
 */
#define POWER_ERROR_EPS_SEND_CONFIG_FAILED ((ErrorCode)(MOD_ID_POWER | 6))

/**
 * @brief A call to Eps_set_ocp_state() failed. See DP.EPS.ERROR_CODE for the
 * error cause.
 */
#define POWER_ERROR_EPS_SET_OCP_STATE_FAILED ((ErrorCode)(MOD_ID_POWER | 7))

/**
 * @brief A call to Eps_reset_ocp() failed. See DP.EPS.ERROR_CODE for the error
 * cause.
 */
#define POWER_ERROR_EPS_RESET_OCP_FAILED ((ErrorCode)(MOD_ID_POWER | 8))

/**
 * @brief Couldn't start a new OCP rail reset as one is still in progress.
 */
#define POWER_ERROR_EPS_RESET_OCP_IN_PROGRESS ((ErrorCode)(MOD_ID_POWER | 9))

/**
 * @brief The value of DP.POWER.LAST_EPS_COMMAND is unrecognised by the Power
 * app.
 */
#define POWER_ERROR_UNRECOGNISED_LAST_EPS_COMMAND ((ErrorCode)(MOD_ID_POWER | 10))

#endif /* H_POWER_ERRORS_H */