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
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Timeout for a graceful tansition to complete.
 * 
 * If the timout passes before all apps have gracefully stopped they will be
 * forced to stop.
 * 
 * TODO: should this be configurable?
 */
#define OPMODEMANAGER_GRACE_TRANS_TIMEOUT_S ((double)(1.0))

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

/**
 * @brief Step the graceful transition substate machine.
 * 
 * @return True on success, false on error. See DP.OPMODEMANAGER.ERROR_CODE for
 * cause. 
 */
bool OpModeManager_step_graceful_transition(void);

/**
 * @brief Perform the init sequence for the Boot Up (BU) mode.
 * 
 * Note: returns void as any errors are handled by this function explicitly.
 */
void OpModeManager_bu_init(void);

/**
 * @brief Step function of the BU mode.
 * 
 * @return True on success, false on error. See DP.OPMODEMANAGER.ERROR_CODE for
 * cause.  
 */
bool OpModeManager_bu_step(void);

#endif /* H_OPMODEMANAGER_PRIVATE_H */