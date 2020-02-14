/* 
    Author: Richard A, 2019
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

#include "../component/led.h"
#include "../component/fram.h"

#include "../utility/debug.h"
#include "../utility/byte_plexing.h"

#define CAMERA_FUNC_TEST
// empirically determined
#define CAMERA_TIMEOUT 3000

static uint8_t CAMERA_RESET_CMD[] = {0x56, 0x00, 0x26, 0x00};
// static uint8_t CAMERA_RESET_RES[] = {0x76, 0x00, 0x26, 0x00};
// static uint8_t CAMERA_INIT_END[] = {0x35, 0x32, 0x35, 0x0D, 0x0A, 0x49, 0x6E, 0x69, 0x74, 0x20, 0x65, 0x6E, 0x64, 0x0D, 0x0A};
static uint8_t CAMERA_INIT_END[] = {0x0D, 0x0A, 0x49, 0x6E, 0x69, 0x74, 0x20, 0x65, 0x6E, 0x64, 0x0D, 0x0A};
// this part of camera init was consistantly captured, never have seen the 0x35 0x32

static uint8_t CAMERA_TAKE_PICTURE_CMD[] = {0x56, 0x00, 0x36, 0x01, 0x00};
static uint8_t CAMERA_TAKE_PICTURE_RES[] = {0x76, 0x00, 0x36, 0x00, 0x00};

static uint8_t CAMERA_READ_JPG_SIZE_CMD[] = {0x56, 0x00, 0x34, 0x01, 0x00};
static uint8_t CAMERA_READ_JPG_SIZE_RES[] = {0x76, 0x00, 0x34, 0x00, 0x04, 0x00}; // expect 3 more bytes describing size

static uint8_t CAMERA_READ_JPG_FILE_CMD[] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00};
// next is initial address (3 bytes) then 0x00, then data length (3 bytes) then interval (2 bytes)
static uint8_t CAMERA_READ_JPG_FILE_RES[] = {0x76, 0x00, 0x32, 0x00, 0x00};
static uint8_t CAMERA_PAGE_HEADER[] = {0xFF, 0xD8};
static uint8_t CAMERA_PAGE_FOOTER[] = {0xFF, 0xD9};

static uint8_t CAMERA_SET_RESOLUTION_160x120[] = {0x56, 0x00, 0x54, 0x01, 0x22};
static uint8_t CAMERA_SET_RESOLUTION_RES[] = {0x76, 0x00, 0x54, 0x00, 0x00};



#ifdef CAMERA_FUNC_TEST
    char output[100];
#endif


uint8_t* get_n_bytes_from_camera(uint32_t n_bytes){
    uint8_t* return_buffer;
    uint32_t characters_collected = 0;
    return_buffer = malloc(sizeof(uint8_t)*n_bytes);
    if (return_buffer == NULL){
        #ifdef CAMERA_FUNC_TEST
            debug_print("Failed to allocate memory for camera response");
        #endif
        return NULL;
    }
    for (characters_collected=0; characters_collected<n_bytes; characters_collected++){
        return_buffer[characters_collected] = UART_getc(UART_CAMERA);
    }
    return return_buffer;
}

void send_to_camera(int8_t* buffer, uint32_t length){
    // blocking call might miss the start of the sequence? Try nonblocking
    UART_putb(UART_CAMERA, buffer, length);
}

void shuffle_array(uint8_t* buffer, uint32_t length){
    int position;
    for (position = 1; position < length; position++){
        buffer[position-1] = buffer[position];
    }
}

uint8_t await_response(uint8_t * expected_result, unsigned int result_length){
    uint64_t timer_start_time;
    uint32_t n_correct_recieved = 0;
    uint8_t current_byte;
    uint8_t buffer[result_length];

    #ifdef CAMERA_FUNC_TEST
        debug_print("\tlistening for bytes...");
        UART_puts(UART_INTERFACE, "\t");
    #endif
    RTC_getTime_ms(&timer_start_time);
    while (!RTC_timerElapsed_ms(timer_start_time, CAMERA_TIMEOUT)){
        // if there are no characters (this check important, without it, values are just spat out)
        if (!UART_charsAvail(UART_CAMERA)){
            // pass
        }
        // if there is atleast 1 character
        else if(UART_getc_nonblocking(UART_CAMERA, &current_byte)){
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
    // Delay_ms(3000);

    // documentation suggests changing resolution and then resetting.
    #ifdef CAMERA_FUNC_TEST
        debug_print("--- Sending Resolution command ---");
    #endif
    send_to_camera(CAMERA_SET_RESOLUTION_160x120, sizeof(CAMERA_SET_RESOLUTION_160x120));
    await_response(CAMERA_SET_RESOLUTION_RES, sizeof(CAMERA_SET_RESOLUTION_RES));

    /*
    This was impirically determined to be false. If the reset command is set,
    the response is always init end, sans the 0x3Y part at the start.
    // reset command. This is not the same as power cycling.
    // #ifdef CAMERA_FUNC_TEST
    //     debug_print("--- Sending camera reset command ---");
    // #endif
    // send_to_camera(CAMERA_RESET_CMD, sizeof(CAMERA_RESET_CMD));
    // was_sequence_recieved = await_response(CAMERA_RESET_RES, sizeof(CAMERA_RESET_RES));
    */
}


void demo_take_picture(){
    // third, take picture, await response

    #ifdef CAMERA_FUNC_TEST
        debug_print("--- Sending Take picture command ---");
    #endif
    send_to_camera(CAMERA_TAKE_PICTURE_CMD, sizeof(CAMERA_TAKE_PICTURE_CMD));
    await_response(CAMERA_TAKE_PICTURE_RES, sizeof(CAMERA_TAKE_PICTURE_RES));
}

uint8_t* demo_get_jpeg_size_response(){
    uint32_t jpeg_size_response_length = 3;
    uint32_t jpeg_size;
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


void demo_retrieve_picture (){
    // fifth is where the fun begins
}

int main(void){
    uint32_t jpeg_size;
    uint8_t *jpeg_size_buffer;
    char output[100];
    
    Board_init();
    enable_watchdog_kick();
    RTC_init();
    debug_init();
    watchdog_update = 0xFF;

    #ifdef CAMERA_FUNC_TEST
        debug_print("=== Camera demo ===");   
    #endif

    demo_init_camera();

    demo_take_picture();

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
    // demo_retrieve_picture();

    LED_off(LED_B);
    #ifdef CAMERA_FUNC_TEST
        debug_print("--- End Demo ---");
    #endif
    while (1){
        watchdog_update = 0xFF;
    }
}

