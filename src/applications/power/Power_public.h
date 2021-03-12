/**
 * @file Power_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application public header
 * 
 * Task ref: [UT_2.11.6]
 * 
 * 
 * 
 * @version 0.1
 * @date 2021-02-23
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_POWER_PUBLIC_H
#define H_POWER_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "applications/power/Power_dp_struct.h"
#include "applications/power/Power_errors.h"
#include "applications/power/Power_events.h"

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief The configuration which defines which OCP rail is enabled in which
 * OpMode.
 * 
 * This is a bit vector summarising the table in OpModePowerTable_Working.xlsm.
 * Use the Power_get_ocp_rail_for_op_mode() funcion to map between this value
 * and Eps_OcpState values.
 */
typedef uint32_t Power_OpModeOcpStateConfig;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the Power app.
 * 
 * @return bool True on success, false on error. Check DP.POWER.ERROR_CODE for
 * the cause of an error.
 */
bool Power_init(void);

/**
 * @brief Step the Power app.
 * 
 * @return bool True on success, false on error. Check DP.POWER.ERROR_CODE for
 * the cause of an error.
 */
bool Power_step(void);

/**
 * @brief Requests that the Power app update the EPS housekeeping packet
 * outside of the standard task execution.
 * 
 * This is useful for acquiring EPS HK data before any tasks are started - i.e.
 * in BU init. The event EVT_EPS_NEW_HK_DATA will be emmitted when the data is
 * available, and is stored in DP.EPS.HK_DATA. Do not poll for this event, as
 * it is used by the Power app too.
 */
void Power_request_eps_hk(void);

/**
 * @brief Sends a request to the EPS to change the OCP state to be ready for
 * the next OPMODE.
 * 
 * This function must be infallible as it is called as a part of the emergency
 * opmode transition.
 */
void Power_request_ocp_state_for_next_opmode(void);

#endif /* H_POWER_PUBLIC_H */