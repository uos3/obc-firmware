/**
 * @file Eps_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief EPS Component Module
 * 
 * Task ref: [UT_2.10.7]
 * 
 * The EPS component provides a command and telemetry interface for the EPS
 * microcontroller (MCU) on board the EPS board. 
 * 
 * @version 0.1
 * @date 2021-02-15
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_EPS_PUBLIC_H
#define H_EPS_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "components/eps/Eps_dp_struct.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialises the EPS module.
 * 
 * @return bool True on success, false on error. See DP.EPS.ERROR_CODE for the
 * cause of the error.
 */
bool Eps_init(void);

#endif /* H_EPS_PUBLIC_H */