/**
 * @ingroup gnulinux
 * @ingroup rtc
 *
 * @file rtc.c
 *
 * @{
 */

#include "board.h"
#include "../rtc.h"

#include <time.h>

void RTC_init(void)
{
	return;
}

void RTC_getTime(uint32_t *time)
{
	struct timespec tp;

	if(clock_gettime(CLOCK_REALTIME, &tp) != 0)
	{
		*time = 0;
		return;
	}

	*time = ((uint32_t)tp.tv_sec);
}

void RTC_getTime_ms(uint64_t *time_ms)
{
	struct timespec tp;

	if(clock_gettime(CLOCK_REALTIME, &tp) != 0)
	{
		*time_ms = 0;
		return;
	}

	*time_ms = ((uint64_t)tp.tv_sec * 1000 + (uint64_t)tp.tv_nsec / 1000000);
}

void RTC_setTime(uint32_t *time)
{
	(void) time;

	return;
}

bool RTC_timerElapsed_ms(uint64_t start_time_ms, uint64_t period_ms)
{
	uint64_t current_time_ms;
	RTC_getTime_ms(&current_time_ms);

	if(current_time_ms < start_time_ms || current_time_ms > (start_time_ms + period_ms))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @}
 */