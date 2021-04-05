/**
 * @file OpModeManager_bu.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief OpModeManager Boot Up (BU) mode initialisations
 * 
 * Task ref: [UT_2.9.10]
 * 
 * @version 0.1
 * @date 2021-04-01
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdbool.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/opmode_manager/OpModeManager_public.h"
#include "system/opmode_manager/OpModeManager_private.h"
#include "drivers/rtc/Rtc_public.h"
#include "drivers/timer/Timer_public.h"
#include "applications/power/Power_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

#ifdef F_BU_SHORT_DWELL_TIME
/**
 * @brief Expected dwell time before begining antenna deploy procedure, in
 * milliseconds.
 * 
 * This is a shorter dwell time than standard to simplify testing. To enable
 * this pass the bu_short_dwell_time feature into the build script.
 */
#define OPMODEMANAGER_BU_DWELL_TIME_MS (5000)
#else
/**
 * @brief Expected dwell time before begining antenna deploy procedure, in
 * milliseconds.
 * 
 * Nominally this is 45 minutes.
 */
#define OPMODEMANAGER_BU_DWELL_TIME_MS (2700000)
#endif

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void OpModeManager_bu_init(void) {
    bool dwell_time_ellapsed;
    ErrorCode error;
    MemStoreManager_PersistentData pers_data;
    Rtc_Timestamp epoch = 0;
    Rtc_Timespan dwell_timespan = Rtc_timespan_from_ms(
        OPMODEMANAGER_BU_DWELL_TIME_MS
    );

    /* Get power to collect some HK data so we can check for low battery */
    Power_request_eps_hk();

    /* Check if the RTC value has passed the expected dwell time */
    Rtc_is_timespan_ellapsed(
        &epoch,
        &dwell_timespan,
        &dwell_time_ellapsed
    );

    /* These procedures require access to the EEPROM data, so we get a copy
     * now */
    pers_data = MemStoreManager_get_pers_data();
    
    /* If the dwell time hasn't ellapsed then start the timer and go straight
     * to the normal operation */
    if (!dwell_time_ellapsed) {
        DEBUG_INF("Dwell time not ellapsed, starting timer");
        error = Timer_start_one_shot(
            (double)(OPMODEMANAGER_BU_DWELL_TIME_MS)/1000.0,
            &DP.OPMODEMANAGER.BU_DWELL_TIMER_EVENT
        );

        /* If we get an error starting the timer we will use the RTC instead */
        if (error != ERROR_NONE) {
            DEBUG_WRN(
                "Couldn't start timer for dwell period: 0x%04X",
                error
            );
            DEBUG_INF("RTC value will be used instead");
            DP.OPMODEMANAGER.BU_DWELL_CHECK_RTC = true;
        }
        else {
            /* If there wasn't an error make sure the check rtc flag is false 
             */
            DP.OPMODEMANAGER.BU_DWELL_CHECK_RTC = false;
        }

        return;
    }
    /* If the antenna has deployed we store EVT_NONE in the dwell timer event,
     * which indicates to the step function that the timer has already passed 
     * and it shouldn't check for its completion */
    else {
        DEBUG_INF("Dwell time already ellapsed, continuing BU exit");
        DP.OPMODEMANAGER.BU_DWELL_TIMER_EVENT = EVT_NONE;
    }
    
    /* Check the pers data to see if the antenna hasn't been deployed */
    if (!pers_data.antenna_deployed) {
        DEBUG_INF("Antenna not deployed, setting to AD");
        /* Transition to AD mode, by mocking a TC request */
        DP.OPMODEMANAGER.NEXT_OPMODE = OPMODEMANAGER_OPMODE_ANTENNA_DEPLOY;
        DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE = true;
        return;
    }

    /* Check if the last mode was safe, if so transition into safe by mocking a
     * TC */
    if (pers_data.last_opmode == OPMODEMANAGER_OPMODE_SAFE) {
        DEBUG_INF("Previous OPMODE was SAFE, reentering SAFE");
        DP.OPMODEMANAGER.NEXT_OPMODE = OPMODEMANAGER_OPMODE_SAFE;
        DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE = true;
        return;
    }
    
    /* BU init complete */
}

bool OpModeManager_bu_step(void) {
    bool dwell_time_ellapsed = false;
    bool eps_new_hk;
    Rtc_Timestamp epoch = 0;
    Rtc_Timespan dwell_timespan = Rtc_timespan_from_ms(
        OPMODEMANAGER_BU_DWELL_TIME_MS
    );

    /* Can either check timer or RTC for the dwell time, which allows the timer
     * module to fail and still enforce the dwell time requirement. */
    if (DP.OPMODEMANAGER.BU_DWELL_CHECK_RTC) {
        /* Check if the RTC value has passed the expected dwell time */
        Rtc_is_timespan_ellapsed(
            &epoch,
            &dwell_timespan,
            &dwell_time_ellapsed
        );
    }
    /* If we're using timer rather than RTC, and there's a valid event in the
     * DP poll for it. */
    else if (DP.OPMODEMANAGER.BU_DWELL_TIMER_EVENT != EVT_NONE) {
        dwell_time_ellapsed = EventManager_poll_event(
            DP.OPMODEMANAGER.BU_DWELL_TIMER_EVENT
        );
    }
    /* If we're not doing any dwell time check (i.e. no RTC and no event) we
     * have already passed the dwell time in the init function, but since we've
     * gone into step the other options (AD/SM) haven't happened. We therefore
     * have to check for low battery voltage before we can transition to NF. */
    else {
        /* Check to see if power has got the new HK data */
        eps_new_hk = EventManager_is_event_raised(EVT_EPS_NEW_HK_DATA);

        /* If we have new HK check to see if power is requesting LP */
        if (eps_new_hk && DP.POWER.LOW_POWER_MODE_REQUEST) {
            /* In BU FDIR will not raise the LP or SM events when requested by
             * applications, since these modes are special. Therefore we raise
             * the FDIR event here instead */
            /* TODO: raise event */
        }
        else if (eps_new_hk) {
            DEBUG_INF("LP check passed, transition to NF");
            /* Otherwise if there's some new HK and we aren't supposed to be in
             * LP mode transition to NF */
            DP.OPMODEMANAGER.NEXT_OPMODE 
                = OPMODEMANAGER_OPMODE_NOMINAL_FUNCTIONING;
            DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE = true;
        }
    }

    /* If the dwell time has ellapsed */
    if (dwell_time_ellapsed) {
        DEBUG_INF("Dwell time ellapsed, transition to AD");

        /* Switch to AD mode by mocking a TC */
        DP.OPMODEMANAGER.NEXT_OPMODE = OPMODEMANAGER_OPMODE_ANTENNA_DEPLOY;
        DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE = true;
    }

    return true;
}