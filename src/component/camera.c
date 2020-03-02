#include <stdlib.h>

#include "../driver/board.h"
#include "../driver/uart.h"
#include "../driver/rtc.h"
#include "../driver/delay.h"

#include "../utility/debug.h"

// #include "camera.h"

// reset command, and reset response. reset response cannot be read
static uint8_t CAMERA_RESET_CMD[] = {0x56, 0x00, 0x26, 0x00};
static uint8_t CAMERA_RESET_RES[] = {0x76, 0x00, 0x26, 0x00};
// actually occurs instead of teh reset
static uint8_t CAMERA_INIT_END[] = {0x0D, 0x0A, 0x49, 0x6E, 0x69, 0x74, 0x20, 0x65, 0x6E, 0x64, 0x0D, 0x0A};

// command to take and response to picture taking
static uint8_t CAMERA_TAKE_PICTURE_CMD[] = {0x56, 0x00, 0x36, 0x01, 0x00};
static uint8_t CAMERA_TAKE_PICTURE_RES[] = {0x76, 0x00, 0x36, 0x00, 0x00};

// read JPEG size
static uint8_t CAMERA_GET_SIZE_CMD[] = {0x56, 0x00, 0x34, 0x01, 0x00};
static uint8_t CAMERA_GET_SIZE_RES[] = {0x76, 0x00, 0x34, 0x00, 0x04, 0x00};

// next is initial address (4 bytes) then data length (4 bytes) then interval (2 bytes)
static uint8_t CAMERA_GET_PAGE_CMD[] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A};
static uint8_t CAMERA_GET_PAGE_RES[] = {0x76, 0x00, 0x32, 0x00, 0x00};

// static uint8_t CAMERA_GET_PAGE_LEN[] = {0x00, 0x00, 0x08, 0x00};
// static uint8_t CAMERA_GET_PAGE_INTERVAL[] = {0x00, 0x0A};

static uint8_t CAMERA_JPEG_HEADER[] = {0xFF, 0xD8};
static uint8_t CAMERA_JPEG_FOOTER[] = {0xFF, 0xD9};

// this flushes the buffer among other things
static uint8_t CAMERA_STOP_TAKING_CMD[] = {0x56, 0x00, 0x36, 0x01, 0x03};
static uint8_t CAMERA_STOP_TAKING_RES[] = {0x76, 0x00, 0x36, 0x00, 0x00};

// one byte for the 
static uint8_t CAMERA_SET_RESOLUTION_CMD[] = {0x56, 0x00, 0x54, 0x01};
static uint8_t CAMERA_SET_RESOLUTION_RES[] = {0x76, 0x00, 0x54, 0x00, 0x00};

// resolution settings
#define CAMERA_RESOLUTION_160x120 0x22
#define CAMERA_RESOLUTION_320x240 0x11
#define CAMERA_RESOLUTION_640x480 0x00
#define CAMERA_RESOLUTION_800x600 0x1D
#define CAMERA_RESOLUTION_1024x768 0x1C
#define CAMERA_RESOLUTION_1280x960 0x1B
#define CAMERA_RESOLUTION_1600x1200 0x21


#define CAMERA_TIMEOUT 2000

/* 
	--------------------------------------------------------------------------
	utility functions
	--------------------------------------------------------------------------
*/

bool do_while_timed(bool (function)(void)){
	// in python, this is obvious, just also pass kwargs.
	// maybe stdarg.h has a solution.
}


bool camera_getbytes(uint8_t result_buffer[], uint32_t expected_length){
	// this, or similar, could probably be added to the UART driver, as there
	// isn't a non-blocking get bytes* function
	uint64_t timer_start_time;
	uint32_t collected_length = 0;
	uint8_t current_byte;

	RTC_getTime_ms(timer_start_time);
	while (!RTC_timerElapsed_ms(timer_start_time, CAMERA_TIMEOUT)){
	// while the timer has not expired
		while(UART_charsAvail(UART_CAMERA)){
			// while there are characters available
			if (UART_getc_nonblocking(UART_CAMERA, &current_byte)){
				// if there is a character available (and also get the character)
				// assign it to the buffer
				result_buffer[collected_length] = current_byte;
				// increase the number of collected characters
				collected_length++;
				// if the collected = expected, return true
				if (collected_length == expected_length){
					return true;
				}
			}
		}
	}
	return false;
}


bool camera_getb_timeout(uint8_t result_byte){
	uint64_t timer_start_time;
	uint32_t collected_length = 0;

	RTC_getTime_ms(timer_start_time);
	while (!RTC_timerElapsed_ms(timer_start_time, CAMERA_TIMEOUT)){
	// while the timer has not expired
		while(!UART_charsAvail(UART_CAMERA)){
			// while there aren't characters; do nothing.
			Delay_us(100);
		}
		if (UART_getc_nonblocking(UART_CAMERA, &result_byte)){
			// if there is a character, return it.
			return true;
		}
	}
	return false;
}


bool camera_awaitbytes(uint8_t expected_data[], uint32_t expected_length){
	uint64_t timer_start_time;
	uint8_t current_byte;
	uint32_t collected_length = 0;
	uint8_t buffer[expected_length];
	#ifdef CAMERA_FUNC_TEST
		debug_print("\t| listening for bytes...");
	#endif
	RTC_getTime_ms(&timer_start_time);
	while (!RTC_timerElapsed_ms(timer_start_time, CAMERA_TIMEOUT))	{
		// if there are no characters (this check important, without it, values are just spat out)
		// wait for the first letter
		while(UART_charsAvail(UART_CAMERA)){
			// if there is atleast 1 character
			if (UART_getc_nonblocking(UART_CAMERA, &current_byte)){
				if (current_byte == expected_data[collected_length]){
					// increase buffer
					buffer[collected_length] = current_byte;
					collected_length++;
					#ifdef CAMERA_FUNC_TEST
						debug_printf( "\t| 0x%02X ", current_byte);
					#endif
				}
				else{
					#ifdef CAMERA_FUNC_TEST
						debug_printf("\t| 0x%02X (incorrect, resetting)", current_byte);
					#endif
					collected_length = 0;
				}
				if (collected_length == expected_length){
					#ifdef CAMERA_FUNC_TEST
						debug_print("\t| listening done (and expected was recieved)");
					#endif
					return true;
				}
			}
		}
	}
	#ifdef CAMERA_FUNC_TEST
		debug_print("\t| listening done (timeout occured)");
	#endif
	return false;
}

void camera_init(){
	UART_init(UART_CAMERA, 38400);
}

void camera_send(int8_t data[], uint32_t data_len){
	// blocking call might miss the start of the sequence? Try nonblocking
	UART_putb(UART_CAMERA, data, data_len);
}

/* 
	--------------------------------------------------------------------------
	camera command functions
	--------------------------------------------------------------------------
*/

bool camera_cmd_noopts(uint8_t command, uint8_t response[]){
	uint32_t cmd_size = sizeof(command); // this might not work, requires testing
	uint32_t res_size = sizeof(response);
	#ifdef CAMERA_FUNC_TEST
		debug_printf("response size: %d", size);
	#endif
	camera_send(command, cmd_size);
	return camera_awaitbytes(response, res_size);
}

bool camera_cmd_withopts(uint8_t command[], uint8_t response[], uint8_t options, uint32_t optlen){
	camera_send(command, sizeof(command));
	camera_send(options, optlen);
	return camera_awaitbytes(response, sizeof(response));
}


/*
	stops the camera from taking pictures (it isn't automatic), and flushes the
	image buffer. Do after recoverying the picture. 

	returns:
	bool: true if the response was recieved, false if not.
*/
bool camera_stop(){
	return camera_cmd_noopts(CAMERA_STOP_TAKING_CMD, CAMERA_STOP_TAKING_RES);
}

/*
	resets the camera; wipes previous activity

	returns:
	bool: true if the response was recieved, false if not.
*/
bool camera_reset(){
	return camera_cmd_noopts(CAMERA_RESET_CMD, CAMERA_RESET_RES);
}

/*
	changes the camera's resolution. volitile; must be done after reset

	args:
	uint8_t resolution_byte: the byte that describes the resolution.

	returns:
	bool; true if the response was recieved, false if not.
*/
bool camera_set_resolution(uint8_t resolution_setting){
	return camera_cmd_withopts(CAMERA_SET_RESOLUTION_CMD, CAMERA_SET_RESOLUTION_RES, &resolution_setting, 1);
}
