/* 
    Author: Richard A, 2019
    Camera demo: takes a picture, dumps it into FRAM (ignoring the blocks) and
    then dumps the FRAM over UART. 
    
*/
#include "../firmware.h"
#include <stdio.h>
#include <stdlib.h>

//#define PAGE_LENGTH 5000 //Buffer_slot_size is 3392bytes, do changed the page size!
#define PAGE_LENGTH 106
#define FUNC_TEST

static char LK_RESET[]     = {0x56, 0x00, 0x26, 0x00};
static char LK_RESET_RE[]    = {0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};
static char LK_RESOLUTION_1600[] = {0x56, 0x00, 0x54, 0x01, 0x21};
static char LK_RESOLUTION_RE[] = {0x76, 0x00, 0x54, 0x00, 0x00}; // v T; response from the camera after setting the resolution
static char LK_COMPRESSION[] = {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04, 0x10}; // Value in last byte
static char LK_COMPRESSION_RE[] = {0x76, 0x00, 0x31, 0x00, 0x00}; // value in last byte
static char LK_PICTURE[]   = {0x56, 0x00, 0x36, 0x01, 0x00};
static char LK_PICTURE_RE[]    = {0x76, 0x00, 0x36, 0x00, 0x00};
static char LK_STOP[] = {0x56, 0x00, 0x36, 0x01, 0x03};
static char LK_STOP_RE[] = {0x76, 0x00, 0x36, 0x00, 0x00};
static char LK_JPEGSIZE[]    = {0x56, 0x00, 0x34, 0x01, 0x00};
static char LK_JPEGSIZE_RE[] = {0x76, 0x00, 0x34, 0x00, 0x04}; //, 0x00, 0x00, 0x00, 0x00}; // then XH XL // assumption is last 4 bytes are size
static char LK_READPICTURE[] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00};
static char LK_PICTURE_TIME_dot1ms[]  = {0x00, 0x0a}; // 0.1 ms
static char LK_READPICTURE_RE[]  = {0x76, 0x00, 0x32, 0x00, 0x00};
static char LK_RESOLUTION_160[] = {0x56, 0x00, 0x54, 0x01, 0x22};
static char LK_RESOLUTION_320[] = {0x56, 0x00, 0x54, 0x01, 0x11};
static char LK_RESOLUTION_640[] = {0x56, 0x00, 0x54, 0x01, 0x00};
static char LK_RESOLUTION_800[] = {0x56, 0x00, 0x54, 0x01, 0x1D};


void demo_init_UART(){
    /* 
        Inits the UARTs needed for the demo,
        DEBUG_4 for output, baud = 115200
        CAMERA for the titular camera, baud = 38400
    */
    UART_init(UART_DEBUG_4, 115200);
    UART_init(UART_CAMERA, 38400);
    #ifdef FUNC_TEST
        UART_puts(UART_DEBUG_4, "\n=== Camera demo ===\n");
    #endif
}


void demo_init_camera(){
    UART_putb(UART_CAMERA, LK_RESET, sizeof(LK_RESET));
}


int main(void){
    // init the uarts
    demo_init_UART();
    // issue take picture command to camera

    // read from camera and dump to fram

    // read from fram and dump over UART

}

