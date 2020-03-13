#include <stdint.h>
#include <stdbool.h>
#include "time.h"
#include "../driver/rtc.h"

void RTC_init(void){
}

void RTC_getTime(uint32_t *time){
	*time = (uint32_t) clock()/CLOCKS_PER_SEC;
}

void RTC_getTime_ms(uint64_t *time_ms){
	*time_ms = (uint64_t) clock()/CLOCKS_PER_SEC*1000;
}

void RTC_setTime(uint32_t *time){
}

bool RTC_timerElapsed_ms(uint64_t start_time_ms, uint64_t period_ms){
	clock_t current_time;
	RTC_getTime_ms(&current_time);
	if ((uint64_t) current_time >= start_time_ms){
		return true;
	}
	return false;
}
