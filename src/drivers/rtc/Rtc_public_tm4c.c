/**
 * @file Rtc_public_tm4c.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief TM4C target Rtc module public implementation.
 * 
 * Task ref: [UT_2.8.4]
 * 
 * @version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

/* External includes */
#include "driverlib/sysctl.h"
#include "driverlib/hibernate.h"

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/delay/Delay_public.h"
#include "drivers/rtc/Rtc_public.h"
#include "drivers/rtc/Rtc_private.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Rtc_init(void) {
    uint8_t checks;
    bool ready;

    /* Enable the peripheral */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);

    /* Check the peripheral has been enabled */
    for (checks = 0; checks < RTC_MAX_NUM_PERIPH_READY_CHECKS; ++checks) {
        ready = SysCtlPeripheralReady(SYSCTL_PERIPH_HIBERNATE);

        if (ready) {
            break;
        }
    }

    /* If the peripheral was enabled within the required number of checks then
     * we can continue, if not we will raise an error */
    if (!ready) {
        return RTC_ERROR_PERIPHERAL_NOT_INIT;
    }

    /* Configure the hibernate system */
    HibernateEnableExpClk(SysCtlClockGet());

    /* Give the oscillator time to stabilise */
    Delay_ms(RTC_OSCILLATOR_STABILISE_DELAY_MS);

    /* Enable the hibernate to use the external oscillator. We use HIGHDRIVE as
     * the filter capacitors on the TOBC 32KHZ crystal are 24 pF, not 12 pF. */
    HibernateClockConfig(HIBERNATE_OSC_HIGHDRIVE);

    /* Enable the RTC clock */
    HibernateRTCEnable();

    /* Set the RTC to 0 seconds */
    HibernateRTCSet(0);
}

Rtc_Timestamp Rtc_get_timestamp(void) {
    /* The RTC value itself holds seconds, while the subsecond holds the values
     * at the right prescision for the timestamp */
    return ((uint64_t)HibernateRTCGet() * RTC_SEC_TO_SUBSEC) 
        + 
        ((uint64_t)HibernateRTCSSGet());
}

void Rtc_set_seconds(uint32_t seconds_in) {
    HibernateRTCSet(seconds_in);
}