/*
*   New camera driver
*   Trying to eliminate the errors in the first one
*   Written from the beginning
*
*   July 2019
*   Maciej Dudziak
 */

#include "board.h"
#include "../uart.h"
#include "../rtc.h"
#include "../delay.h"
#include "../camera_new.h"


//-------------------Camera commands-----------------------------
//start/reset commands
//static char LK_INIT_END[]={0x36, 0x32, 0x35, 0x0D, 0x0A, 0x49, 0x6E, 0x69, 0x74, 0x20, 0x65, 0x6E, 0x64, 0x0D, 0x0A};
static char LK_RESET[] = {0x56, 0x00, 0x26, 0x00};
static char LK_RESET_RE[] = {0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};
//baudrate setting
//last two bytes ([5],[6]) for baudrate choice
// 　0xAE  :   9600
// 　0x2A  :   38400
// 　0x1C  :   57600
//   0x0D  :   115200
// 　0xAE  :   128000
// 　0x56  :   256000
//from the example code, may be actually different - reffer to datasheet
static char LK_BAUDRATE[] = {0x56, 0x00, 0x24, 0x03, 0x01, 0x00};
static char LK_BAUDRATE_RE[] = {0x76, 0x00, 0x24, 0x00, 0x00};
//image size setting - in last [4] byte
// 0x22 : 160*120
// 0x11 : 320*240
// 0x00 : 640*480
// 0x1D : 800*600
// 0x1C : 1024*768
// 0x1B : 1280*960
// 0x21 : 1600*1200
static char LK_RESOLUTION[] = {0x56, 0x00, 0x54, 0x01, 0x00};
static char LK_RESOLUTION_RE[] = {0x76, 0x00, 0x54, 0x00, 0x00};
//take picture command
static char LK_PICTURE[] = {0x56, 0x00, 0x36, 0x01, 0x00};
static char LK_PICTURE_RE[] = {0x76, 0x00, 0x36, 0x00, 0x00};
//read jpeg size command; last four bytes of the response is the picture size
static char LK_JPEGSIZE[] = {0x56, 0x00, 0x34, 0x01, 0x00};
static char LK_JPEGSIZE_RE[] = {0x76, 0x00, 0x34, 0x00, 0x04};
//compression commands
static char LK_COMPRESSION[] = {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04, 0x00}; // Value in last byte
static char LK_COMPRESSION_RE[] = {0x76, 0x00, 0x31, 0x00, 0x00, 0x00}; // value in last byte
//read picture data commands
static char LK_READPICTURE[] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00};
static char LK_PICTURE_interval[] = {0x00, 0x0A};   //0.1ms LK_PICTURE_interval
static char LK_READPICTURE_RE[] = {0x76, 0x00, 0x32, 0x00, 0x00};
static char JPEG_START[] = {0xFF, 0xD8};
static char JPEG_END[] = {0xFF, 0xD9};


void set_resolution(uint16_t resolution){
    switch (resolution)
    {
    case 160:
        LK_RESOLUTION[4] = 0x22;
        break;
    case 640:
        LK_RESOLUTION[4] = 0x00;
    case 1280:
        LK_RESOLUTION[4] = 0x1B;
    case 1600:
        LK_RESOLUTION[4] = 0x21;
    default:
        LK_RESOLUTION[4] = 0x21;
        break;
    }
}
#define write_command(command) UART_putb(UART_CAMERA, command, sizeof(command));

static bool command_match(char *string, uint32_t string_length){
    char data_in;
    uint8_t index = 0;
    Delay_ms(25);       //delay required for the response characters to appear in UART FIFO buffer, estimated empirically
    while(UART_charsAvail(UART_CAMERA)){
            UART_getc_nonblocking(UART_CAMERA, &data_in);
        if(data_in == string[index]){
            index++;
        }else
        {
            index = 0;
        }
    }
    if(index == string_length){return true; }
    else{return false; }
}

static uint32_t UART_getw4(uint8_t serial)
{
  char c1=UART_getc(serial),c2=UART_getc(serial),c3=UART_getc(serial),c4=UART_getc(serial);
  return (uint32_t)((c1 << 24) | (c2 << 16) | (c3 << 8) | (c4 << 0)); //shifting values of read char/hex numbers to create one 32bit number
}


bool Camera_capture1(){
    uint16_t rest, pagesize = 104;      //BUFFER_SLOT_SIZE/8 = 106, but it need to be multiple integer of 8
    uint32_t jpegsize, number_of_pages;
    uint8_t page_buffer[pagesize];
    bool endflag = false;

    UART_init(UART_CAMERA, 38400);

    write_command(LK_RESET);                            //write reset command
    if(!command_match(LK_RESET_RE, sizeof(LK_RESET_RE))){return false; }     //return false if response not matched
    Delay_ms(3000);
    //set resolution
    set_resolution(1600);
    write_command(LK_RESOLUTION);
    if(!command_match(LK_RESOLUTION_RE, sizeof(LK_RESOLUTION_RE))){ return false;}
    //set compression rate
    set_compression(0x10);
    write_command(LK_COMPRESSION);
    if(!command_match(LK_COMPRESSION_RE,sizeof(LK_COMPRESSION_RE))){ return false;}
    //command picture taking   
    write_command(LK_PICTURE);
    if(!command_match(LK_PICTURE_RE,sizeof(LK_PICTURE_RE))){ return false;}
    //read jpeg size
    write_command(LK_JPEGSIZE);
    if(!command_match(LK_JPEGSIZE_RE, sizeof(LK_JPEGSIZE_RE))){ return false;}
    //updating values of variables
    jpegsize = UART_getw4(UART_CAMERA);
    number_of_pages = jpegsize / pagesize;
    rest = jpegsize % pagesize;

    LK_READPICTURE[13] = pagesize;
    LK_READPICTURE[14] = LK_PICTURE_interval[0];
    LK_READPICTURE[15] = LK_PICTURE_interval[1];
    //read the data
while(!endflag){
    //read command
    write_command(LK_READPICTURE);
    //match first five starting characters
    if(!command_match(LK_READPICTURE_RE, sizeof(LK_READPICTURE_RE))){ return false;}
    for(int j = 0; j<pagesize; j++){
        page_buffer[j] = UART_getc(UART_CAMERA);
        if((page_buffer[j-1]==0xFF)&&(page_buffer[j]==0xD9)) endflag = true;    //set endflag true when find the end of jpeg
    }
    if(!command_match(LK_READPICTURE_RE, sizeof(LK_READPICTURE_RE))){ return false;}
    Buffer_store_new_data(page_buffer);
    UART_putb(UART_GNSS, page_buffer, sizeof(page_buffer));  //print to UART UART_GNSS
}


}

