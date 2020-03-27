#include <stdint.h>
#include <string.h>

#include "../utility/debug.h"
#include "../driver/rtc.h"
#include "../driver/delay.h"
#include "../driver/watchdog_ext.h"

#define MSG_BUF_SIZE 256
#define WAIT_PERIOD 10*1000
// #define FUNC_TEST

int delay_test(){
	uint64_t start_time, end_time;
	// debug_print("DEMO: running test. Please wait. (30s)");
	RTC_getTime_ms(&start_time);
	// wait 30 seconds using delay
	Delay_ms(WAIT_PERIOD);
	RTC_getTime_ms(&end_time);
	// print after to *hopefully* prevent delay
	// debug_printf("DEMO: delay start (ms):% 15llu", start_time);
	// debug_printf("DEMO: delay end   (ms):% 15llu", end_time);
	debug_printf("DEMO: delay diff   (s):% 15lf", ((double) (end_time-start_time))/1000);
	return 0;
}

int timer_elapsed_test(){
	uint64_t start_time, end_time;
	// debug_print("DEMO: running test. Please wait. (30s)");
	RTC_getTime_ms(&start_time);
	// wait 30 seconds using delay

	while(!RTC_timerElapsed_ms(start_time, WAIT_PERIOD)){
		// fast wait with no delay
		Delay_us(100);
	}
	RTC_getTime_ms(&end_time);
	// print after to *hopefully* prevent delay
	// debug_printf("DEMO: timer start (ms):% 15llu", start_time);
	// debug_printf("DEMO: timer end   (ms):% 15llu", end_time);
	debug_printf("DEMO: timer diff   (s):% 15lf", ((double) (end_time-start_time))/1000);
	return 0;
}


int run_test(int (*test_fn)(void)){
	char _msg_buffer[MSG_BUF_SIZE];
	char* msg_buffer = (char*) &_msg_buffer;
	#ifdef FUNC_TEST
		debug_print("| Enter command:");
	#endif
	debug_get_string(msg_buffer, MSG_BUF_SIZE);
	if (strcmp(msg_buffer, "start")==0){
		#ifdef FUNC_TEST
			debug_print("starting...");
		#endif
		debug_flash(3);
		test_fn();
		debug_flash(3);
		return 0;
	}
	else if(strcmp(msg_buffer, "quit")==0){
		#ifdef FUNC_TEST
			debug_print("quiting...");
		#endif
		return -1;
	}
}


int main(){
	int res;


	debug_init();
	RTC_init();

	while (1){
		#ifdef FUNC_TEST
			debug_print("=== RTC timekeeping check ===");
			debug_print("this program is used to check that the RTC doesn't loose time. To be used in conjunction with obc-firmware/tools/timer_check.py");
		#endif

		res = run_test(&delay_test);
		if (res == -1){
			break;
		}
		Delay_ms(1000);
		res = run_test(&timer_elapsed_test);
		if (res == -1){
			break;
		}
		

		#ifdef FUNC_TEST
			debug_print("=== test series complete, restarting ===");
		#endif
		// debug_end();
		Delay_ms(1000);
		watchdog_update = 0xFF;
	}
	// #ifdef FUNC_TEST
		debug_print("=== done ===");
	// #endif
	debug_end();
}
