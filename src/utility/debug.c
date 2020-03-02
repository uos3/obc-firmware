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

#include "debug.h"


void debug_init(){
	// might aswell include board watchdog init
	Board_init();
	enable_watchdog_kick();
	watchdog_update = 0xFF;
	UART_init(UART_INTERFACE, 9600);
	LED_on(LED_B);
}

void debug_print(char* debug_message) {
	#ifdef DEBUG_MODE
		UART_puts(UART_INTERFACE, debug_message); 
		UART_puts(UART_INTERFACE, "\r\n");
	#endif
}

void debug_clear(){
	for (int i = 0; i < 100; i++){
		#ifdef DEBUG_MODE
			UART_puts(UART_INTERFACE, "\r\n");
		#endif
	}
}

void debug_end(){
	LED_off(LED_B);
	while(1){
		watchdog_update = 0xFF;
	}
}

void debug_hex(uint8_t data[], uint32_t data_len){
	char output[6];
	for (int i = 0; i< data_len; i++){
		sprintf(output, "0x%02X ", data[i]);
		UART_puts(UART_INTERFACE, output);
	}
	UART_puts(UART_INTERFACE, "\r\n");
}

void _vdebug_printf(const char f_string[], va_list vars)
{
	char tmp_output[256];
	vsprintf(tmp_output, f_string, vars);
	debug_print(tmp_output);
}

void debug_printf(const char f_string[], ...){
	va_list argp;
	va_start(argp, f_string);
	_vdebug_printf(f_string, argp);
	va_end(argp);
}


void debug_flash(uint8_t n_times){
	for (int i = 0; i< n_times; i++){
		LED_off(LED_B);
		Delay_ms(200);
		LED_on(LED_B);
		Delay_ms(200);
	}
}


uint32_t debug_get_string(char output[], uint32_t max_output_length){
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
			output[recieved] = current_character;
			recieved ++;
		}
	}
	// replace last character with null terminator
	output[recieved] = '\0';
	return recieved+1;
}

int debug_parse_int(char buffer[], int n_characters){
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


uint32_t debug_get_command(char output[], uint32_t max_output_length){
	uint32_t recieved = 0;
	char happy_with_buffer[2];
	bool happy_with_output = false;
	debug_flush_uart();
	while(!happy_with_output){
		debug_printf("| Enter a command: ");
		recieved = debug_get_string(output, max_output_length);
		debug_printf("| you have entered: '%s'. is this correct? (y/n)", output);
		// debug_hex(output, recieved);
		debug_get_string(happy_with_buffer, 2);
		debug_printf("| entered %s", happy_with_buffer);
		if (happy_with_buffer[0] == 'y'){
			happy_with_output = true;
		}
	}
	return recieved;
}


