/**
 * @ingroup uart
 * 
 * @file Uart_events.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Defines the events available for the UART module.
 * 
 * 
 * @version 0.1
 * @date 2021-04-07
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_UART_EVENTS_H
#define H_UART_EVENTS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_module_ids.h"
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_NONE ((Event)(MOD_ID_EVENTMANAGER | 0))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_GNSS_RX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 1))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_GNSS_TX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 2))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_CAM_RX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 3))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_CAM_TX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 4))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_EPS_RX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 5))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_EPS_TX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 6))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_TEST_RX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 7))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_TEST_TX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 8))

#endif /* H_UART_EVENTS_H */