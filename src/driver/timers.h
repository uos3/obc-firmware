/*
	provides an interface for the GPTM on the TM4C.
*/

#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

/*
	Sets up the timer config for mission use. Must be called before
	any other timer function.
*/
void timers_init();


/*
	Assigns a handler to a specific timer. After the timer expires, it will
	automatically start again

	Args
		uint32_t timer_num
		the number of the timer to be used. For TM4C, this can only be 0-5.
*/
uint8_t timers_setup_timer(uint32_t timer_num, uint64_t period_s, void (*handler_fnptr)(void));

/*
	Unregisters handlers and disables timers
*/
void timers_stop_all();


#endif // TIMERS_H