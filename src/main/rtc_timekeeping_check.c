#include <stdint.h>

#include "../utility/debug.h"
#include "../driver/rtc.h"

int main(){
	debug_init();
	RTC_init();
	debug_print("=== RTC timekeeping check ===");
	uint64_t start_time;
	// get the current time
	RTC_getTime(&start_time);
	debug_printf("DEMO: start time recorded as: %llu", start_time);



	debug_print("=== done ===");
	debug_end();
}
