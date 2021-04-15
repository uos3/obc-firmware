/**
 * @file Power_dp_struct.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application DataPool struct definition
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_POWER_DP_STRUCT_H
#define H_POWER_DP_STRUCT_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdbool.h>

/* Internal includes */
#include "system/kernel/Kernel_errors.h"
#include "system/event_manager/EventManager_public.h"
#include "components/eps/Eps_public.h"
#include "applications/power/Power_public.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Power Application DataPool structure.
 */
typedef struct _Power_Dp {
    
    /**
     * @brief Indicates if the Power app is initialised (true) or not (false).
     * 
     * @dp 1
     */
    bool INITIALISED;

    /**
     * @brief Stores errors that occur during operation.
     * 
     * @dp 2
     */
    Error ERROR;

    /**
     * @brief Stores errors returned by the Timer driver.
     * 
     * @dp 3
     */
    Error TIMER_ERROR;

    /**
     * @brief Status value of the low power check.
     * 
     * If this value is either POWER_LOW_POWER_STATUS_LOW_POWER the battery
     * voltage is below the threshold to transition into low power. This shall
     * be detected by the FDIR module and an EVT_FDIR_LOW_POWER_REQUEST event
     * will be emmitted, which will trigger OpModeManager to begin an emergency
     * transition to low power mode.
     * 
     * If the value is POWER_LOW_POWER_STATUS_FAULT the voltage of the battery
     * cannot be determined and FDIR shall trigger a low power mode transition
     * as well.
     * 
     * All other values indicate nominal functioning of the system.
     * 
     * @dp 4
     */
    Power_LowPowerStatus LOW_POWER_STATUS;

    /**
     * @brief The event associated with the app's primary task timer.
     * 
     * @dp 5
     */
    Event TASK_TIMER_EVENT;

    /**
     * @brief The requested state of the OCP rails, which is based on the
     * OpMode and the stored OpMode-OCP state configuration vector.
     * 
     * @dp 6
     */
    Eps_OcpState REQUESTED_OCP_STATE;

    /**
     * @brief Flag which when true will cause the Power app to request a new
     * EPS HK packet from the EPS outside of the standard task operation. See
     * Power_request_eps_hk().
     * 
     * This flag is reset when the Power app successfully sends the HK request
     * to the EPS.
     * 
     * @dp 7
     */
    bool UPDATE_EPS_HK;

    /**
     * @brief Flag which when true will cause the Power app to send an updated
     * configuration file to the EPS.
     *
     * @dp 8
     */
    bool UPDATE_EPS_CFG;

    /**
     * @brief Flag which when true will cause the Power app to send an updated
     * OCP state to the EPS.
     * 
     * @dp 9
     */
    bool UPDATE_EPS_OCP_STATE;

    /**
     * @brief The type of command which was last issued to the EPS. 
     * 
     * @dp 10
     */
    Eps_UartDataType LAST_EPS_COMMAND;

    /**
     * @brief The number of consecutive EPS command failures. Used to detect
     * possible malfunctions in the EPS.
     * 
     * @dp 11
     */
    uint8_t NUM_CONSEC_FAILED_EPS_COMMANDS;

    /**
     * @brief This flag shall be true if a command to set the EPS OCP state
     * succeeds. If the EPS fails to return the expected OCP state, this will
     * be false. It shall also be false from the time a OCP update is
     * requested, until a successful update is detected.
     * 
     * @dp 12
     */
    bool EPS_OCP_STATE_CORRECT;

    /**
     * @brief Flag which is true while the Power app is performing the actions
     * needed to change OPMODE, namely:
     *  - Updating the OCP state of the EPS.
     * 
     * @dp 13
     */
    bool OPMODE_CHANGE_IN_PROGRESS;

    /**
     * @brief Flag which will trigger the EPS to reset the rails which are true
     * in OCP_RAILS_TO_RESET:
     * 
     * @dp 14
     */
    bool SEND_RESET_OCP_TC;

    /**
     * @brief The OCP rails that the EPS should reset. Will only be sent if
     * SEND_RESET_OCP_TC is true. 
     * 
     * @dp 15
     */
    Eps_OcpState OCP_RAILS_TO_RESET;

    /**
     * @brief Flag which will trigger the sending of the battery command stored
     * in BATT_CMD_TO_SEND to the EPS. 
     * 
     * @dp 16
     */
    bool SEND_BATT_TC;

    /**
     * @brief Battery command to send when SEND_BATT_TC is true.
     * 
     * @dp 17
     */
    Eps_BattCmd BATT_CMD_TO_SEND;

} Power_Dp;

#endif /* H_POWER_DP_STRUCT_H */