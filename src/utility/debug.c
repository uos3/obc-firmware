/*
		Debug tools, feel free to expand.

		Author: Richard A
		Last modified: 2020 02 13
		Status: Functional
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "../driver/board.h"

#include "../driver/uart.h"
#include "../driver/watchdog_ext.h"
#include "../component/led.h"
#include "../driver/delay.h"
#include "../driver/rtc.h"

#include "debug.h"

char line_end[] = "\r\n";
char no_line_end[] = "";
char *line_end_ptr;
int do_command_confirm = 0;


void debug_init(){
	// might aswell include board watchdog init
	Board_init();
	enable_watchdog_kick();
	RTC_init();
	line_end_ptr = (char*) &line_end;
	watchdog_update = 0xFF;
	UART_init(UART_INTERFACE, 9600);
	LED_on(LED_B);
}


void debug_clear(){
	for (int i = 0; i < 100; i++){
		#ifdef DEBUG_MODE
			UART_puts(UART_INTERFACE, line_end_ptr);
		#endif
	}
}

void debug_end(){
	LED_off(LED_B);
	while(1){
		watchdog_update = 0xFF;
		// to prevent CPU roasting
		Delay_ms(1000);
	}
}

void debug_enable_newline(){
	line_end_ptr = (char*) &line_end;
}

void debug_disable_newline(){
	line_end_ptr = (char*) &no_line_end;
}

void debug_print(char* debug_message) {
	#ifdef DEBUG_MODE
		UART_puts(UART_INTERFACE, debug_message); 
		UART_puts(UART_INTERFACE, line_end_ptr);
	#endif
}

void debug_hex(uint8_t* data, uint32_t data_len){
	char output[6];
	for (int i = 0; i< data_len; i++){
		sprintf(output, "0x%02X ", data[i]);
		UART_puts(UART_INTERFACE, output);
	}
	UART_puts(UART_INTERFACE, line_end_ptr);
}

void _vdebug_printf(const char* f_string, va_list vars){
	char tmp_output[256];
	vsprintf(tmp_output, f_string, vars);
	debug_print(tmp_output);
}

void debug_printf(const char* f_string, ...){
	va_list argp;
	va_start(argp, f_string);
	_vdebug_printf(f_string, argp);
	va_end(argp);
}

void debug_printl(char* string, uint32_t len){
	debug_disable_newline();
	for (int i = 0; i < len; i++){
		debug_printf("%c", string[i]);
	}
	debug_enable_newline();
	debug_print("");
}

void debug_flash(uint8_t n_times){
	for (int i = 0; i< n_times; i++){
		LED_off(LED_B);
		Delay_ms(200);
		LED_on(LED_B);
		Delay_ms(200);
	}
}


uint32_t debug_get_string(char* output, uint32_t max_output_length){
	uint32_t recieved = 0;
	char current_character = 0;
	bool end_flag = false;
	// -1 to leave space for the null terminator
	while ((!end_flag) && (recieved < (max_output_length-1))){
		while(UART_charsAvail(UART_INTERFACE)){
			current_character = UART_getc(UART_INTERFACE);
			// if the enter key is pressed
			if (current_character == '\r'){
				end_flag = true;
				break;
			}
			if (current_character == '\n'){
				end_flag = true;
				break;
			}
			output[recieved] = current_character;
			recieved ++;
		}
	}
	// replace last character with null terminator
	output[recieved] = '\0';
	return recieved+1;
}

int debug_parse_int(char* buffer, int n_characters){
	int result, i, multiplier, length, digit;
	multiplier = 1;
	length = strlen(buffer);
	i = length-1;
	result = 0;
	// ascii '0 is 48, and ascii '9' is 57, so as long as it is between them,
	// it is a number.
	while	(((int) buffer[i] >= (int) '0') 
			&& ((int) buffer[i] <= (int) '9')
			&& (i >= n_characters)){
		digit = ((uint8_t) buffer[i]) - (int) '0';
		// debug_printf("got digit: %d %c", digit, digit);
		result += multiplier * digit;
		multiplier *= 10;
		i--;
	}
	return result;
}

void debug_flush_uart(){
	while(UART_charsAvail(UART_INTERFACE)){
		UART_getc(UART_INTERFACE);
	}
}


bool debug_command_confirm(char* output){
	char happy_with_buffer[2];
	debug_printf("| you have entered: '%s'. is this correct? (y/n)", output);
	// debug_hex(output, recieved);
	debug_get_string(happy_with_buffer, 2);
	debug_printf("| entered %s", happy_with_buffer);
	if (happy_with_buffer[0] == 'y'){
		return true;
	}
	return false;
}


uint32_t debug_get_command(char* output, uint32_t max_output_length){
	uint32_t recieved = 0;

	bool happy_with_output = false;
	debug_flush_uart();
	while(!happy_with_output){
		debug_printf("| Enter a command: ");
		recieved = debug_get_string(output, max_output_length);
		if (do_command_confirm==1){
			happy_with_output = debug_command_confirm(output);
		}
		else{
			debug_printf("| you have entered: '%s'", output);
			happy_with_output = true;
		}
		Delay_ms(100);
	}
	return recieved;
}

void debug_countdown(int total_countdown_time_ms, uint32_t n_intervals){
	uint64_t start_time;
	uint64_t interval;
	int n_occured = 1;
	line_end_ptr = (char*) &no_line_end;
	interval = total_countdown_time_ms/(n_intervals);
	RTC_getTime_ms(&start_time);
	debug_print("Time elapsed (s): ");
	while(!RTC_timerElapsed_ms(start_time, total_countdown_time_ms)){
		if (RTC_timerElapsed_ms(start_time, interval*n_occured)){
			// debug_printf("%d ", (int) (start_time-warning_time)*1000);
			watchdog_update = 0xFF;
			debug_printf("%d ", ((int) interval)/1000*n_occured);
			n_occured++;
		}
		Delay_ms(interval/100);
	}
	line_end_ptr = (char*) &line_end;
	debug_print("");
}

void debug_marker(char* string){
	debug_disable_newline();
	debug_print("DEMO: ");
	debug_print(string);
	debug_print("...");
	debug_enable_newline();
}

void debug_done(){
	debug_print(" done");
}

