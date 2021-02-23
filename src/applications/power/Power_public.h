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

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief The configuration which defines which OCP rail is enabled in which
 * OpMode.
 * 
 * This is a bit vector summarising the table in OpModePowerTable_Working.xlsm.
 * Use the Power_get_ocp_rail_for_op_mode() funcion to map between this value
 * and another.
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

#endif /* H_POWER_PUBLIC_H */