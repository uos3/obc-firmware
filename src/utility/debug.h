/*
    Debug tools, feel free to expand.

    Author: Richard A
    Last modified: 2020 02 13
    Status: Functional
*/


#ifndef __DEBUG_H__
#define __DEBUG_H__

// toggle comment when not debugging
#define DEBUG_MODE

#include "../driver/board.h"

// protection against not having it defined
#ifdef DEBUG_MODE
    #define UART_INTERFACE UART_DEBUG_4
#endif

void debug_init();

void debug_clear();

void debug_end();

void debug_enable_newline();

void debug_disable_newline();

// prints data as hex digits, displayed 0xAA
void debug_hex(uint8_t* data, uint32_t data_len);

// prints a null terminated string
void debug_print(char* debug_message);

// like printf, but over the UART
void debug_printf(const char f_string[], ...);

// prints a non null terminated string.
void debug_printl(char* string, uint32_t len);


void debug_flash(uint8_t n_times);

uint32_t debug_get_string(char* output, uint32_t max_output_length);

uint32_t debug_get_command(char* output, uint32_t max_output_length);

int debug_parse_int(char* buffer, int n_characters);


void debug_countdown(int total_countdown_time_ms, uint32_t n_intervals);

#endif /*  __DEBUG_H__ */
