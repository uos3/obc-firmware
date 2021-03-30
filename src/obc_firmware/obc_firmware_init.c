/**
 * @file obc_firmware_init.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Central OBC firmware initialisation functions
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
#include "drivers/rtc/Rtc_public.h"

/* System */
#include "system/kernel/Kernel_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"

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

void obc_firmware_init_early_modules(void) {
    ErrorCode error = ERROR_NONE;

    /* ---- MODULE INITIALISATION ---- */

    /* Init the critical modules required for basic operation:
     *  - DataPool
     *  - Board (system clock rate)
     *  - Debug (basic logging)
     *  - EventManager
     *  - Fdir
     * 
     * A failure in any of these is critical, and the recovery strategy is
     * currently unclear.
     */
    Kernel_init_critical_modules();

    /* Init rtc driver. This is done as early as possible to get the RTC time
     * as close to reboot as possible. It is possible that the RTC module could
     * be unresponsive, in which case it wouldn't be available to provide
     * critical timing information. This would be bad and there's not many ways
     * we can recover from this. */
    error = Rtc_init();
    if (error != ERROR_NONE) {
        DEBUG_ERR("Rtc_init failed!");
        /* TODO: Inform FDIR about failure */
    }

    /* After critical modules want to initialise the config provider next */
    error = Eeprom_init();
    if (error != ERROR_NONE) {
        /* EEPROM can fail to initialise for two reasons:
         *  - The number of read/write cycles has been exceeded
         *  - The power is unstable
         * 
         * If the number of read/write cycles is exceeded there's nothing we
         * can do. If it's a power instability giving the system some time to
         * stabilise and then resetting may help, but to do this we really need
         * to keep the system going. If this happens we will therefore attempt
         * to load the redundant configuration which is kept as a part of the
         * software image. We do this by raising the
         * DP.MEMSTOREMANAGER.USE_BACKUP_CFG flag.
         */
        DP.MEMSTOREMANAGER.USE_BACKUP_CFG = true;
        DEBUG_WRN(
            "EEPROM initialisation failed, MemStoreManager will load backup config file"
        );
    }

    /* Init MemStoreManager, which can fail if the EventManager encounters an
     * error. If some cfg files are corrupted the defaults will be loaded,
     * which itself can't fail. */
    if (!MemStoreManager_init()) {
        /* TODO: actions on event manager failure */
    }

    /* TODO: Init watchdog driver */

    /* TODO: init delay driver */

    /* Init Timer driver. If this fails scheduling tasks won't work, but the
     * anything else could continue as normal. This means that much of the
     * functionality of the system will be broken, but up/downlink _might_
     * still be able to continue. Therefore failure to init timer doesn't
     * constitute a complete failure of the entire mission. */
    error = Timer_init();
    if (error != ERROR_NONE) {
        /* TODO: raise timer disabled FDIR event? */
    }

    /* TODO: init udma driver */

    /* Init GPIO driver */
    /* TODO: fix GPIO init
    error = Gpio_init();
    if (error != ERROR_NONE) {

    }*/

    /* TODO: Init uart driver */

    /* Init I2C driver. If this fails only those components dependent on I2C
     * are affected, unfortunately this includes the temp monitoring app, which
     * then only leaves the battery temperature as the only indicator. This
     * failure is dealt with at the app level, but we want to generate an FDIR
     * event for observability. */
    error = I2c_init();
    if (error != ERROR_NONE) {
        /* TODO: Raise I2C init failed FDIR event */
    }

    /* TODO: init SPI driver */
}

void obc_firmware_init_system(void) {
    /* TODO: init service interface */

    /* TODO: init TM manager */

    /* TODO: init TC router */

    /* TODO: init parameter monitor */

    /* Init OpModeManager */
    OpModeManager_init();

    /* TODO: init service handlers */
}

void obc_firmware_init_components(void) {

    /* LED doesn't have an init function */
    
    /* Init EPS */
    if (!Eps_init()) {
        /* If the EPS init fails we have a major problem. The EPS system should
         * start with all the rails required for SAFE mode on, so if we have
         * this problem we must immediately raise the transition to safe event.
         * 
         * There's a chance that the problem could be fixed by the ground
         * control changing config, so we want to give them that possibility by
         * allowing commands and telemetry through still, which can be done in
         * safe mode. */
        /* TODO: raise FDIR safe mode event */
    }

    /* TODO: init radio */

    /* TODO: init FRAM */

    /* TODO: init Antenna */

    /* TODO: init Camera */

    /* TODO: init GNSS */
    
    /* Init IMU. Failures in IMU could be from event manager or from a critical
     * module not being initialised. */
    if (!Imu_init()) {
        /* TODO: Raise error tm */
    }

    /* TODO: init temp */
}

void obc_firmware_init_applications(void) {

    /* Init power */
    if (!Power_init()) {
        /* Power can only fail to init if a critical module isn't init. We will
         * do nothing as getting to this stage should be impossible. */
    }

    /* TODO: Init thermal */

    /* TODO: init comms */

    /* TODO: init deployment */

    /* TODO: init picture */

    /* TODO: init payload */

    /* TODO: init beacon */
}