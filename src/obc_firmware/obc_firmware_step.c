/**
 * @file obc_firmware_step.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Central OBC firmware step functions
 * 
 * @version 0.1
 * @date 2021-03-11
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

/* Utility */
#include "util/debug/Debug_public.h"

/* Drivers */
#include "drivers/board/Board_public.h"
#include "drivers/uart/Uart_public.h"
#include "drivers/timer/Timer_public.h"
#include "drivers/eeprom/Eeprom_public.h"
#include "drivers/gpio/Gpio_public.h"
#include "drivers/uart/Uart_public.h"

/* System */
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/mem_store_manager/MemStoreManager_public.h"
#include "system/opmode_manager/OpModeManager_public.h"

/* Components */
#include "components/eps/Eps_public.h"
#include "components/led/Led_public.h"
#include "components/imu/Imu_public.h"

/* Apps */
#include "applications/power/Power_public.h"

/* Firmware */
#include "obc_firmware/obc_firmware.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void obc_firmware_step_drivers(void) {
    /* GPIO has no step function, as it is effectively stateless */
    
    /* Step I2C */
    if (!I2c_step()) {
        /* TODO: register error with FDIR */
    }

    /* UART has no step, as it is handled by the UDMA */

    /* TODO: step SPI */
}

void obc_firmware_step_components(void) {

    /* Step EPS */
    if (!Eps_step()) {
        /* TODO: register error with FDIR */
    }

    /* TODO: step radio */

    /* TODO: step FRAM */

    /* TODO: step Antenna */

    /* TODO: step Camera */

    /* TODO: step GNSS */
    
    /* Step IMU */
    if (!Imu_step()) {
        /* TODO: Raise error tm */
    }

    /* TODO: step temp */

}

void obc_firmware_step_system(void) {

    /* Step power */
    if (!Power_step()) {
        /* TODO: register error with FDIR */
    }

    /* TODO: step thermal */

    /* TODO: step comms */

    /* TODO: step TC router */

    /* TODO: step TM manager (if it has a step func?) */

    /* TODO: step parameter monitor */

    /* TODO: step opmode manager */
    if (!OpModeManager_step()) {
        /* TODO: register error with FDIR */
    }
}