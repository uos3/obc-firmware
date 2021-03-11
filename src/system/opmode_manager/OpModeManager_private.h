/**
 * @file OpModeManager_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief OpModeManager private header
 * 
 * Task ref: [UT_2.9.10]
 * 
 * @version 0.1
 * @date 2021-03-10
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_OPMODEMANAGER_PRIVATE_H
#define H_OPMODEMANAGER_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdbool.h>

/* Internal includes */
#include "system/opmode_manager/OpModeManager_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Activate all user apps required for the current OpMode.
 */
void OpModeManager_activate_apps(void);

/**
 * @brief Check the apps (both user and system) for any mode transition 
 * requests.
 * 
 * @return True on success, false on error. See DP.OPMODEMANAGER.ERROR_CODE for
 * cause. 
 */
bool OpModeManager_check_app_trans_reqs(void);

/**
 * @brief Call the step functions for all active apps in the current opmode.
 * 
 * Note: System apps are stepped by the Kernel, not the OpModeManager.
 * 
 * @return True on success, false on error. See DP.OPMODEMANAGER.ERROR_CODE for
 * cause. 
 */
bool OpModeManager_call_active_app_steps(void);

#endif /* H_OPMODEMANAGER_PRIVATE_H */