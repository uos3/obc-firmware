/**
 * @ingroup uos3-proto
 * @ingroup rtc
 *
 * @file rtc.c
 *
 * @{
 */

#include "board.h"
#include "../rtc.h"
#include "../delay.h"

/* Required for 'struct tm' prototypes in driverlib/hibernate.h */
#include <time.h>

#include "driverlib/sysctl.h"
#include "driverlib/hibernate.h"

void RTC_init(void)
{
	if(HibernateIsActive() == false)
	{
		/* Enable Hibernation Peripheral */
		HibernateEnableExpClk(SysCtlClockGet());

		/* Give time for 32KHz Crystal Osc to stabilise */
		Delay_ms(1500);

		/* Enable 32KHz Osc as RTC Clock */
		HibernateClockConfig(HIBERNATE_OSC_HIGHDRIVE);

		/* Enable RTC */
	  HibernateRTCEnable();
	}
}

void RTC_getTime(uint32_t *time)
{
	*time = HibernateRTCGet();
}

void RTC_getTime_ms(uint64_t *time_ms)
{
	*time_ms = ((uint64_t)HibernateRTCGet() * 1000) + (((uint64_t)HibernateRTCSSGet() * 1000) / 32768);
}

void RTC_setTime(uint32_t *time)
{
	HibernateRTCSet(*time);
}

/**
 * @}
 */