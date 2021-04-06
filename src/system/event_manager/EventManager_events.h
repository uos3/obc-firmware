/**
 * @ingroup event_manager
 * 
 * @file EventManager_events.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides possible events for the EventManager.
 * 
 * @version 2.0
 * @date 2020-11-02
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_EVENTMANAGER_EVENTS_H
#define H_EVENTMANAGER_EVENTS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "system/kernel/Kernel_module_ids.h"
#include "system/event_manager/EventManager_public.h"
#include "system/mem_store_manager/MemStoreManager_events.h"
#include "drivers/i2c/I2c_events.h"
#include "drivers/timer/Timer_events.h"
#include "components/imu/Imu_events.h"

/* -------------------------------------------------------------------------   
 * EVENTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Event associated with an event manager error.
 * 
 * This event indicates that an error has occured in the EventManager, and the
 * corresponding error code in the EventManager's DataPool
 * DP.EVENTMANAGER.ERROR_CODE has been set.
 * 
 * Note, if the error arises from already being at the maximum number of events
 * the error code will be set and the corresponding
 * DP.EVENTMANAGER.MAX_EVENTS_REACHED flag will be raised. This event will not
 * be raised.
 */
#define EVT_EVENTMANAGER_ERROR ((Event)(MOD_ID_EVENTMANAGER | 1))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_GNSS_RX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 2))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_GNSS_TX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 3))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_CAM_RX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 4))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_CAM_TX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 5))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_EPS_RX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 6))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_EPS_TX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 7))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_TEST_RX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 8))

/**
 * @brief Event associated with the uDMA completing a transfer over either
 * UART or SPI.
 * 
 */
#define EVT_UART_TEST_TX_COMPLETE ((Event)(MOD_ID_EVENTMANAGER | 9))


#endif /* H_EVENTMANAGER_EVENTS_H */