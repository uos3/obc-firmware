/**
 * @file OpModeManager_private.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief OpModeManager private implementations
 * 
 * Task ref: [UT_2.9.10]
 * 
 * @version 0.1
 * @date 2021-03-11
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdbool.h>
#include <stdint.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "system/opmode_manager/OpModeManager_public.h"
#include "system/opmode_manager/OpModeManager_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void OpModeManager_activate_apps(void) {
    int i;
    Kernel_AppId app_id;
    
    /* Loop through all app Ids that should be in the current opmode, and
     * activate each one */
    for (i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
        /* Get the app id at this index */
        app_id = CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.OPMODE][i];

        /* If the app ID is zero it is empty, so we skip to the next */
        if (app_id == 0) {
            continue;
        }

        /* Otherwise switch over the possible user apps, and call their
         * activate functions */
        switch (app_id) {
            case APP_ID_BEACON:
                DEBUG_TRC("Activating Beacon app");
                /* TODO: call activate func when defined */
                break;
                
            case APP_ID_DEPLOYMENT:
                DEBUG_TRC("Activating Deployment app");
                /* TODO: call activate func when defined */
                break;

            case APP_ID_PAYLOAD:
                DEBUG_TRC("Activating Payload app");
                /* TODO: call activate func when defined */
                break;

            case APP_ID_PICTURE:
                DEBUG_TRC("Activating Picture app");
                /* TODO: call activate func when defined */
                break;

            default:
                DEBUG_WRN(
                    "OPMODE %d contains an AppId 0x%02X unkown to OpModeManager",
                    DP.OPMODEMANAGER.OPMODE,
                    app_id
                );
                /* TODO: raise error event? */
                break;
        }
    }
}

bool OpModeManager_check_app_trans_reqs(void) {
    
    /* 
     * System app transition requests
     * 
     * Generally speaking system apps don't request graceful transitions, they
     * use the FDIR system to request emergency ones - i.e. Power will request
     * LP mode through the FDIR events.
     * 
     * However this doesn't preclude some system app requesting a transition
     * at some point in the future - if we decide to keep DL mode then comms
     * will be responsible for requesting it. 
     */

    /* 
     * User app transition requests
     * 
     * As any emergency transition requests will be made through the FDIR
     * system we are only checking for graceful transitions. The way each app
     * requests a graceful transition should be through the event system. In
     * addition a check is made that the app is supposed to be running in that
     * mode, this is a safeguard against poorly written apps attempting to take
     * control of the OpModeManager.
     */

    /* TODO: For all user apps:
     *  - Check for transition request event (evt is app specfic)
     *  - Check the app is supposed to be running in the current mode
     *  - Check that the transition is allowed?
     *  - Set the state into graceful transition and set the next opmode
     */

    return true;
}

bool OpModeManager_call_active_app_steps(void) {
    int i;
    Kernel_AppId app_id;

    /* For all apps active in the current mode call their step functions */
    for (i = 0; i < OPMODEMANAGER_MAX_NUM_APPS_IN_MODE; ++i) {
        /* Get the app id at this index */
        app_id = CFG.OPMODE_APPID_TABLE[DP.OPMODEMANAGER.OPMODE][i];

        /* If the app ID is zero it is empty, so we skip to the next */
        if (app_id == 0) {
            continue;
        }

        switch (app_id) {
            case APP_ID_BEACON:
                DEBUG_TRC("Stepping Beacon app");
                /* TODO: call step func when defined */
                break;
                
            case APP_ID_DEPLOYMENT:
                DEBUG_TRC("Stepping Deployment app");
                /* TODO: call step func when defined */
                break;

            case APP_ID_PAYLOAD:
                DEBUG_TRC("Stepping Payload app");
                /* TODO: call step func when defined */
                break;

            case APP_ID_PICTURE:
                DEBUG_TRC("Stepping Picture app");
                /* TODO: call step func when defined */
                break;

            default:
                DEBUG_WRN(
                    "OPMODE %d contains an AppId 0x%02X unkown to OpModeManager",
                    DP.OPMODEMANAGER.OPMODE,
                    app_id
                );
                /* TODO: raise error event? */
                break;
        }
    }

    return true;
}