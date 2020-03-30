#include "../driver/timers.h"
#include "../driver/delay.h"
#include "../utility/debug.h"


int interrupts_involked = 0;


void handler_test(void){
	interrupts_involked += 1;
	debug_print("DEMO: handler function fired");
}

void multi_handler_test0(void){
	debug_print("DEMO: handler 0 fired");
}


void multi_handler_test1(void){
	debug_print("DEMO: handler 1 fired");
}


void multi_handler_test2(void){
	debug_print("DEMO: handler 2 fired");
}


void multi_handler_test3(void){
	debug_print("DEMO: handler 3 fired");
}


void multi_handler_test4(void){
	debug_print("DEMO: handler 4 fired");
}

void multi_handler_test5(void){
	debug_print("DEMO: handler 5 fired");
}

void (*handler_list[])(void) = {
	multi_handler_test0,
	multi_handler_test1,
	multi_handler_test2,
	multi_handler_test3,
	multi_handler_test4,
	multi_handler_test5
};


int main(){
	debug_init();
	debug_clear();
	debug_print("=== Timer interrupt demo ===");
	debug_print("this demo demonstates the timer-interrupt feature of the TOBC");

	debug_marker("inniting timers");
	timers_init();
	debug_done();

	// single timer functionality test

	debug_marker("setting interrupt handler for 5s intervals");
	timers_setup_timer(0, 5, &handler_test);
	debug_done();

	debug_print("DEMO: Entering wait phase for interrupts:");
	Delay_ms(6000);
	debug_printf("DEMO: got %d interrupts", interrupts_involked);
	interrupts_involked = 0;

	debug_marker("clearing all timers.");
	timers_stop_all();
	debug_done();
	debug_marker("waiting to confirm (no more timers should fire)");
	Delay_ms(6000);
	debug_done();
	debug_printf("DEMO: got %d interrupts", interrupts_involked);
	interrupts_involked = 0;

	// testing all the timers

	debug_print("DEMO: testing all the timers");
	for (int timer = 0; timer < 6; timer++){
		timers_setup_timer(timer, timer/10 + 1, handler_list[timer]);
	}
	Delay_ms(2000);
	debug_print("should have fired 6 handlers, 0-5, in ascending order");
	timers_stop_all();

	debug_print("=== demo end ===");
	debug_end();
}


