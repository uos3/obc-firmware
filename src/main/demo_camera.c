/* 
	Author: Richard A
	Last modified: 2020 02 14
	Status: functional file size reading.
	Camera demo: takes a picture, dumps it into FRAM (ignoring the blocks) and
	then dumps the FRAM over UART. 
	
*/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../driver/board.h"
#include "../driver/uart.h"
#include "../driver/rtc.h"
#include "../driver/delay.h"
#include "../driver/watchdog_ext.h"
#include "../driver/wdt.h"
#include "../driver/rtc.h"
// #include "../driver/fram.h"

#include "../component/led.h"

#include "../utility/debug.h"
#include "../utility/byte_plexing.h"

// #define CAMERA_FUNC_TEST
// #define CAMERA_FSIZE_COMP
// empirically determined
#define CAMERA_TIMEOUT 5000

static uint8_t CAMERA_RESET_CMD[] = {0x56, 0x00, 0x26, 0x00};
// static uint8_t CAMERA_RESET_RES[] = {0x76, 0x00, 0x26, 0x00};
// static uint8_t CAMERA_INIT_END[] = {0x35, 0x32, 0x35, 0x0D, 0x0A, 0x49, 0x6E, 0x69, 0x74, 0x20, 0x65, 0x6E, 0x64, 0x0D, 0x0A};
static uint8_t CAMERA_INIT_END[] = {0x0D, 0x0A, 0x49, 0x6E, 0x69, 0x74, 0x20, 0x65, 0x6E, 0x64, 0x0D, 0x0A};
// this part of camera init was consistantly captured, never have seen the 0x35 0x32

static uint8_t CAMERA_TAKE_PICTURE_CMD[] = {0x56, 0x00, 0x36, 0x01, 0x00};
static uint8_t CAMERA_TAKE_PICTURE_RES[] = {0x76, 0x00, 0x36, 0x00, 0x00};

static uint8_t CAMERA_READ_JPG_SIZE_CMD[] = {0x56, 0x00, 0x34, 0x01, 0x00};
static uint8_t CAMERA_READ_JPG_SIZE_RES[] = {0x76, 0x00, 0x34, 0x00, 0x04, 0x00}; // expect 3 more bytes describing size

// next is initial address (4 bytes) then data length (4 bytes) then interval (2 bytes)
static uint8_t CAMERA_READ_JPG_FILE_CMD[] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A};
// probably want some text in here
static uint8_t CAMERA_READ_JPG_FILE_RES[] = {0x76, 0x00, 0x32, 0x00, 0x00};
static uint8_t CAMERA_READ_JPG_FILE_RES_ALT[] = {0x76, 0x76, 0x00, 0x32, 0x00};
// 2048 bytes. Nice number. In theory, min res file is ~1450 bytes.
static uint8_t CAMERA_READ_JPG_FILE_DATA_LEN[] = {0x00, 0x00, 0x08, 0x00};
static uint8_t CAMERA_READ_JPG_FILE_DATA_INTERVAL[] = {0x00, 0x0A};

static uint8_t CAMERA_PAGE_HEADER[] = {0xFF, 0xD8};
static uint8_t CAMERA_PAGE_FOOTER[] = {0xFF, 0xD9};

static uint8_t CAMERA_STOP_TAKING[] = {0x56, 0x00, 0x36, 0x01, 0x03};
static uint8_t CAMERA_STOP_TAKING_RES[] = {0x76, 0x00, 0x36, 0x00, 0x00};

static uint8_t CAMERA_SET_RESOLUTION_1600x1200[] = {0x56, 0x00, 0x54, 0x01, 0x21};
static uint8_t CAMERA_SET_RESOLUTION_160x120[] = {0x56, 0x00, 0x54, 0x01, 0x22};
static uint8_t CAMERA_SET_RESOLUTION_RES[] = {0x76, 0x00, 0x54, 0x00, 0x00};


char output[100];

uint8_t *get_n_bytes_from_camera(uint32_t n_bytes)
{
	uint8_t *return_buffer;
	uint32_t characters_collected = 0;
	return_buffer = malloc(sizeof(uint8_t) * n_bytes);
	if (return_buffer == NULL){
			#ifdef CAMERA_FUNC_TEST
		debug_print("Failed to allocate memory for camera response");
		#endif
		return NULL;
	}
	for (characters_collected = 0; characters_collected < n_bytes; characters_collected++){
		return_buffer[characters_collected] = UART_getc(UART_CAMERA);
	}
	return return_buffer;
}

void send_to_camera(int8_t *buffer, uint32_t length){
	// blocking call might miss the start of the sequence? Try nonblocking
	UART_putb(UART_CAMERA, buffer, length);
}

void shuffle_array(uint8_t *buffer, uint32_t length){
	int position;
	for (position = 1; position < length; position++)
	{
		buffer[position - 1] = buffer[position];
	}
}

uint8_t await_response(uint8_t *expected_result, unsigned int result_length){
	uint64_t timer_start_time;
	uint32_t n_correct_recieved = 0;
	uint8_t current_byte;
	uint8_t buffer[result_length];
	#ifdef CAMERA_FUNC_TEST
		debug_print("\tlistening for bytes...");
		UART_puts(UART_INTERFACE, "\t");
	#endif
	RTC_getTime_ms(&timer_start_time);
	while (!RTC_timerElapsed_ms(timer_start_time, CAMERA_TIMEOUT))	{
		// if there are no characters (this check important, without it, values are just spat out)
		if(!UART_charsAvail(UART_CAMERA)){
			continue;
		}
		// if there is atleast 1 character
		if (UART_getc_nonblocking(UART_CAMERA, &current_byte)){
			if (current_byte == expected_result[n_correct_recieved]){
			#ifdef CAMERA_FUNC_TEST
				sprintf(output, "0x%02X ", current_byte);
				UART_puts(UART_INTERFACE, output);
			#endif
				buffer[n_correct_recieved] = current_byte;
				n_correct_recieved++;
			}
			else{
				#ifdef CAMERA_FUNC_TEST
					sprintf(output, "0x%02X (incorrect, resetting) \r\n\t", current_byte);
					UART_puts(UART_INTERFACE, output);
				#endif
				n_correct_recieved = 0;
			}
			if (n_correct_recieved == result_length){
				#ifdef CAMERA_FUNC_TEST
					debug_print("\r\n\tlistening done (and expected was recieved)");
				#endif
				return 0xFF;
			}
		}
	}
	#ifdef CAMERA_FUNC_TEST
		debug_print("\r\n\tlistening done (timeout occured)");
	#endif
	return 0;
}

void demo_init_camera(){
	uint8_t was_sequence_recieved;
	// init the uart
	UART_init(UART_CAMERA, 38400);
	// check for init sequence
	#ifdef CAMERA_FUNC_TEST
		debug_print("--- Waiting for init sequence ---");
	#endif

	send_to_camera(CAMERA_RESET_CMD, sizeof(CAMERA_RESET_CMD));
	// sending to camera seems neccisary, only spits out the init end after prodding.
	was_sequence_recieved = await_response(CAMERA_INIT_END, sizeof(CAMERA_INIT_END));
	// delay of 2-3s was suggested by documentation, however it isn't neccisary (apparently)
}


void camera_stop_taking(){
	send_to_camera(CAMERA_STOP_TAKING, sizeof(CAMERA_STOP_TAKING));
	await_response(CAMERA_STOP_TAKING_RES, sizeof(CAMERA_STOP_TAKING_RES));
}


void change_resolution(){
	#ifdef CAMERA_FUNC_TEST
		debug_print("--- Sending Resolution command ---");
	#endif
	send_to_camera(CAMERA_SET_RESOLUTION_1600x1200, sizeof(CAMERA_SET_RESOLUTION_1600x1200));
	// send_to_camera(CAMERA_SET_RESOLUTION_160x120, sizeof(CAMERA_SET_RESOLUTION_160x120));
	await_response(CAMERA_SET_RESOLUTION_RES, sizeof(CAMERA_SET_RESOLUTION_RES));
}


void demo_take_picture(){
	// take picture, await response
	#ifdef CAMERA_FUNC_TEST
		debug_print("--- Sending Take picture command ---");
	#endif
	send_to_camera(CAMERA_TAKE_PICTURE_CMD, sizeof(CAMERA_TAKE_PICTURE_CMD));
	await_response(CAMERA_TAKE_PICTURE_RES, sizeof(CAMERA_TAKE_PICTURE_RES));
}


uint8_t *demo_get_jpeg_size_response(){
	uint32_t jpeg_size_response_length = 3;
	uint32_t jpeg_size;
	// malloc is done by get_n_bytes
	uint8_t *jpeg_size_buffer;
	// fourth, read file size, await response
	#ifdef CAMERA_FUNC_TEST
		debug_print("--- Sending Read filesize command ---");
	#endif
	send_to_camera(CAMERA_READ_JPG_SIZE_CMD, sizeof(CAMERA_READ_JPG_SIZE_CMD));
	await_response(CAMERA_READ_JPG_SIZE_RES, sizeof(CAMERA_READ_JPG_SIZE_RES));
	jpeg_size_buffer = get_n_bytes_from_camera(jpeg_size_response_length);
	return jpeg_size_buffer;
}


// Camera image grabbing



void camera_get_jpg_page_cmd(uint32_t page_start_address){
	uint8_t page_start_address_bytes[4];

	// get page command
	send_to_camera(CAMERA_READ_JPG_FILE_CMD, sizeof(CAMERA_READ_JPG_FILE_CMD));

	// create some bytes here
	data_split_u32_le(page_start_address, page_start_address_bytes);
	send_to_camera(page_start_address_bytes, 4);

	// length to read
	send_to_camera(CAMERA_READ_JPG_FILE_DATA_LEN, sizeof(CAMERA_READ_JPG_FILE_DATA_LEN));

	// interval
	send_to_camera(CAMERA_READ_JPG_FILE_DATA_INTERVAL, sizeof(CAMERA_READ_JPG_FILE_DATA_INTERVAL));
}


uint8_t page_has_footer(uint8_t *page_buffer, uint32_t current_size){
	if (current_size < sizeof(CAMERA_PAGE_FOOTER)){
		return 0x00;
	}
	if (memcmp(&page_buffer[current_size - sizeof(CAMERA_PAGE_FOOTER)], CAMERA_PAGE_FOOTER, sizeof(CAMERA_PAGE_FOOTER))==0){
		return 0xFF;
	}
	else{
		return 0x00;
	}
}


uint8_t page_has_cmd_end(uint8_t *page_buffer, uint32_t current_page_size){
	uint32_t cmd_footer_size;
	uint32_t page_buffer_command_end_index;
	
	cmd_footer_size = sizeof(CAMERA_READ_JPG_FILE_RES);

	if (current_page_size < cmd_footer_size){
		return 0x00;
	}

	page_buffer_command_end_index = current_page_size - 1 - cmd_footer_size;
	#ifdef CAMERA_FUNC_TEST

	#endif

	if (memcmp(&page_buffer[page_buffer_command_end_index], CAMERA_READ_JPG_FILE_RES, cmd_footer_size) == 0)
	{
		return 0xFF;
	}
	else if (memcmp(&page_buffer[page_buffer_command_end_index], CAMERA_READ_JPG_FILE_RES_ALT, cmd_footer_size) == 0)
	{
		return 0xFF;
	}
	else{
		return 0x00;
	}
}


uint32_t pull_camera_page(uint8_t *page_buffer, uint32_t page_length){
	uint32_t collected = 0;
	uint64_t timer_start_time;
	uint8_t current_byte;

	RTC_getTime_ms(&timer_start_time);
	while (!RTC_timerElapsed_ms(timer_start_time, CAMERA_TIMEOUT))
	{
		// if there are no characters (this check important, without it, values are just spat out)
		if (!UART_charsAvail(UART_CAMERA))
		{
			continue;
		}
		// if there is atleast 1 character
		else if (UART_getc_nonblocking(UART_CAMERA, &current_byte))
		// else
		{
			// resets the timeout
			// RTC_getTime_ms(&timer_start_time);
			// current_byte = UART_getc(UART_CAMERA);
			page_buffer[collected] = current_byte;
			collected++;
			if (page_has_cmd_end(page_buffer, collected) == 0xFF)
			{
				return (collected - sizeof(CAMERA_READ_JPG_FILE_RES) - 1);
			}
		}
	}
	// timer has elapsed. This probably means no more bytes are being transmitted.
	#ifdef CAMERA_FUNC_TEST
		debug_print("page pull: timer elapsed");
	#endif
	return collected;
}

uint8_t demo_retrieve_picture(uint32_t jpeg_size){
	#ifdef CAMERA_FUNC_TEST
		debug_print("\r\n--- Picture retrieve start ---");
	#endif
	// header                              Initial address       Read file             Response delay
	// 0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00  0x00 0x00 0x00  0x00  0x00 0x08 0x00  0x00 0x0A

	// the maximum page length
	uint32_t max_page_length = 0;
	// the total number of bytes read from the camera, sans cmd strings
	uint32_t total_read = 0;
	// the number of bytes just read from the camera
	uint32_t page_read = 0;
	// pointer to local page buffer.
	uint8_t *page_buffer;

	// create buffer space for page
	max_page_length = data_combine_u32(CAMERA_READ_JPG_FILE_DATA_LEN);
	#ifdef CAMERA_FUNC_TEST
		sprintf(output, "packed camera read length: %lu", max_page_length);
		debug_print(output);
	#endif

	page_buffer = malloc(max_page_length * sizeof(uint8_t));
	if (page_buffer == NULL)
	{
		return 0x00;
	}

	while (total_read < jpeg_size)	{
		// send the read command
		#ifdef CAMERA_FUNC_TEST
			debug_print("issuing read command");
		#endif
		// read address is the same as the total read
		camera_get_jpg_page_cmd(total_read);
		// wait for correct sign
		await_response(CAMERA_READ_JPG_FILE_RES, sizeof(CAMERA_READ_JPG_FILE_RES));
		// pull the page from the camera
		page_read = pull_camera_page(page_buffer, max_page_length);
		// update total read
		total_read+=page_read;
		// for the test only, dump the pages over UART
		#ifndef CAMERA_FUNC_TEST
			UART_putb(UART_INTERFACE, page_buffer, page_read);
		#endif
		// if it's not in bytes, this needs to change
		#ifdef CAMERA_FUNC_TEST
			sprintf(output, "read page of length: %lu for a total of %lu",page_read, total_read);
			debug_print(output);
		#endif
		// if the page has a footer, we're done
		if (page_has_footer(page_buffer, page_read))
		{
			#ifdef CAMERA_FUNC_TEST
				debug_print("found footer!");
			#endif
			return 0xFF;
		}
		// break;
	}
	if (page_has_footer(page_buffer, page_read)){
		#ifdef CAMERA_FUNC_TEST
			debug_print("found footer!");
		#endif
		return 0xFF;
	}
	#ifdef CAMERA_FUNC_TEST
		debug_print("footer not found");
		// debug_hex(&page_buffer[page_read], page_read);
	#endif
	return 0x00;
}


// main

int main(void)
{
	uint32_t jpeg_size;
	uint8_t *jpeg_size_buffer;
	char output[100];

	debug_init();
	RTC_init();
	watchdog_update = 0xFF;

	#ifdef CAMERA_FUNC_TEST
		debug_print("=== Camera demo ===");
	#endif

	demo_init_camera();

	// recommended in the documentation
	Delay_ms(4000);

	change_resolution();

	// just so you know when it's taking the picture.
	debug_flash(3);

	demo_take_picture();

	debug_flash(3);

	// jpeg_size = demo_read_file_size();
	jpeg_size_buffer = demo_get_jpeg_size_response();
	jpeg_size = data_combine_u24(jpeg_size_buffer);

	#ifdef CAMERA_FUNC_TEST
		sprintf(output, "jpeg size bytes: %02X %02X %02X",
				jpeg_size_buffer[0],
				jpeg_size_buffer[1],
				jpeg_size_buffer[2]
				// jpeg_size_buffer[3]
		);
		debug_print(output);
		sprintf(output, "read jpeg file size as %d bytes", jpeg_size);
		debug_print(output);
	#endif
	#ifdef CAMERA_FSIZE_COMP
		debug_printf("jpeg_size %d", jpeg_size);
	#endif

	demo_retrieve_picture(jpeg_size);

	camera_stop_taking();

	#ifdef CAMERA_FUNC_TEST
		debug_print("=== end demo ===");
	#endif

	debug_end();
}
