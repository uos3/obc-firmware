//
// camera test
//
// Suzanna Lucarotti (c) 22/09/2017
//
// for use with the UoS3 Cubesat
//

#include "../firmware.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include <stdlib.h>
#include <stdarg.h>

//#define DEBUG_SERIAL UART_CAM_HEADER // this is the serial port debug messages will come through (UART_CAM_HEADER or UART_PC104_HEADER)
#define GPS_SERIAL UART_PC104_HEADER
#define CAM_SERIAL UART_CAM_HEADER

#define DEBUG_SERIAL GPS_SERIAL

#define sz(a) a,sizeof(a)


//LK_POWERUP[] = {0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};
char LK_POWERUP[] = {0x0d, 0x0a, 0x00, 0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a}; // Init end
//LK_POWERUP[] = {0x0d,0x0a,0,4,0,0x49,0x68,0x69,0x68,0x20,0x65,0xc,0};

char LK_BAUDRATE_19200[]	={0x56, 0x00, 0x24, 0x03, 0x01, 0x56, 0xe4};
char LK_BAUDRATE_38400[]	= {0x56, 0x00, 0x24, 0x03, 0x01, 0x2a, 0xf2};
char LK_BAUDRATE_RE[]	= {0x76, 0x00, 0x24, 0x00, 0x00};
 
char LK_RESOLUTION_VGA[] = {0x56, 0x00, 0x54, 0x01, 0x00};
char LK_RESOLUTION_800[] = {0x56, 0x00, 0x54, 0x01, 0x1D};
char LK_RESOLUTION_1280[] = {0x56, 0x00, 0x54, 0x01, 0x1B};
char LK_RESOLUTION_1600[] = {0x56, 0x00, 0x54, 0x01, 0x21};
char LK_RESOLUTION_RE[] = {0x76, 0x00, 0x54, 0x00, 0x00}; // v T

char LK_COMPRESSION[] = {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04};
char LK_COMPRESSION_RE[] = {0x76, 0x00, 0x31, 0x00, 0x00}; // v 1

char LK_RESET[] 		= {0x56, 0x00, 0x26, 0x00};
// OLD: LK_RESET_RE[] 		= a2s([0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a])
// LEGIT: LK_RESET_RE[] 		= a2s([0x76,0x00,0x31,0x00,0x00])
char LK_RESET_RE[] 		= {0x0d, 0x0a, 0x49, 0x6e, 0x69, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x0d, 0x0a};

char LK_PICTURE[]		= {0x56, 0x00, 0x36, 0x01, 0x00};
char LK_PICTURE_RE[]		= {0x76, 0x00, 0x36, 0x00, 0x00};
char LK_JPEGSIZE[] 		= {0x56, 0x00, 0x34, 0x01, 0x00};
char LK_JPEGSIZE_RE[]	= {0x76, 0x00, 0x34, 0x00, 0x04}; //, 0x00, 0x00, 0x00, 0x00}; // then XH XL // assumption is last 4 bytes are size
char LK_STOP[]		= {0x56, 0x00, 0x36, 0x01, 0x03};
char LK_STOP_RE[]		= {0x76, 0x00, 0x36, 0x00, 0x00};

char LK_READPICTURE[]	= {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00};
char LK_PICTURE_TIME[]	= {0x00, 0x0A}; // .1 ms
char LK_READPICTURE_RE[] 	= {0x76, 0x00, 0x32, 0x00, 0x00};
char JPEG_START[] 		= {0xFF, 0xD8};
char JPEG_END[]		= {0xFF, 0xD9};


 
// Wrappers for printing to serial port - always to the one specified as output, placed here close to code for clarity

#define DISP3(x,y,z) UART_putstr(DEBUG_SERIAL,x,y,z); // x and z are strings, y is a number shown as signed base10 16bit
#define DISP2(y,z) UART_putstr(DEBUG_SERIAL,NULL,y,z); 
#define DISP1(x) UART_puts(DEBUG_SERIAL,x);

 //////////////////////////////////////////////////////////////////
 // the actual main code
 //////////////////////////////////////////////////////////////////

unsigned int UART_getw(long int serial)
 {
 char c1=UART_getc(serial),c2=UART_getc(serial);
 return (c1<<8)|c2;
 }

unsigned int UART_getw4(long int serial)
 {
 char c1=UART_getc(serial),c2=UART_getc(serial),c3=UART_getc(serial),c4=UART_getc(serial);
 return (c1<<24)|(c2<<16)|(c3<<8)|(c4<<0);
 }


void mem_packet_send (char *start_addr, unsigned int len) // this copied over from mem_debug
 {
 // send byte data from a given address and send down serial line DEBUG_SERIAL
 // will hang processor if address wrong
 // lets put a CRC check on the packet
 // for CRC gonna use xor initially for speed of implementation

 UART_putc(DEBUG_SERIAL,'@');
 
 unsigned char x=0;
 for (char *addr=start_addr;addr<start_addr+len;addr++)
   {
   unsigned int s_addr= (int) start_addr;
	char c=*addr;                    
	UART_putc(DEBUG_SERIAL,c);
	x=x ^ c;
   }
   UART_putc(DEBUG_SERIAL,'|');
   UART_putc(DEBUG_SERIAL,x); // write the check byte 

 }

bool match_string(char *data, unsigned int len) // packed to call with sz(data)
 {
 // DISP1("Matching '");UART_putb(DEBUG_SERIAL,data,len);DISP1("'\n\r");
  for (int i=0;i<len;i++) {  char c=UART_getc(CAM_SERIAL); /*DISP3("*",c,"*");*/  if (c!=data[i]) return false; }
return true;
 }

 unsigned int getcamword()
  {
  	return UART_getw4(CAM_SERIAL);
//	char c1=UART_getc(CAM_SERIAL),c2=UART_getc(CAM_SERIAL),c3=UART_getc(CAM_SERIAL),c4=UART_getc(CAM_SERIAL);
//	return c1<<24|c2<<16|c3<<8|c4;
  }

  void writeword4(int serial,unsigned int w)
   {
	UART_putc(serial,(w>>24)&255);UART_putc(serial,(w>>16)&255);UART_putc(serial,(w>>8)&255);UART_putc(serial,w & 255);
//	DISP3("Just pasted0 ",(w>>24)&255,"\r\n");
//	DISP3("Just pasted1 ",(w>>16)&255,"\r\n");
//	DISP3("Just pasted2 ",(w>>8)&255,"\r\n");
//	DISP3("Just pasted3 ",(w)&255,"\r\n");
   }

  void writecamword(unsigned int w)
   {
	writeword4(CAM_SERIAL,w);
   }

 #define CAMWRITE(a) UART_putb(CAM_SERIAL,a,sizeof(a)); // send this message

 #define CAMREAD(a) while (!match_string(a,sizeof(a))) {UART_putc(DEBUG_SERIAL,'.');} // wait for this sequence (forever if necessary)
 
unsigned int take_picture(char *picture_storage)
 {
   if (((unsigned int) picture_storage)&3!=0) {DISP1("address must be word aligned");return 0;}

   DISP3(" Take Picture Storage Address: ",picture_storage,"\n\r");
  
  CAMWRITE(LK_RESET);CAMREAD(LK_RESET_RE);DISP1("Initialised OK\n\r"); // initialise camera
  CAMWRITE(LK_RESOLUTION_VGA);CAMREAD(LK_RESOLUTION_RE);DISP1("Resolution OK\n\r"); // set resolution
  CAMWRITE(LK_COMPRESSION);UART_putc(CAM_SERIAL,0x10);CAMREAD(LK_COMPRESSION_RE);DISP1("Compression OK\n\r"); // set compression
  CAMWRITE(LK_PICTURE);CAMREAD(LK_PICTURE_RE);DISP1("Take Picture OK\n\r"); // take picture
  CAMWRITE(LK_JPEGSIZE);CAMREAD(LK_JPEGSIZE_RE);unsigned int jpegsize=getcamword(); // file size (lowest 32 bits)
  DISP3("Picture size is ",jpegsize," bytes of image data\n\r");

  CAMWRITE(LK_READPICTURE);UART_putc(CAM_SERIAL,0);UART_putc(CAM_SERIAL,0); // offset in file start (0 here)
  writecamword(jpegsize); // write length to obtain
  CAMWRITE(LK_PICTURE_TIME);
  CAMREAD(LK_READPICTURE_RE);DISP1("PICTURE READ arranged\n\r");

  //char *picture_storage=0x10000; // store it here

  unsigned int pages_to_clear=(jpegsize+1023)>>10; // this processor's flash is in 1k blocks, these need to be erased before writing to
  

  DISP3("Pages to clear",pages_to_clear,"\r\n");
  for (int i=0;i<pages_to_clear;i++)
	FlashErase(picture_storage+(i<<10));

  DISP1("Erased\r\n");

 char flash_word_size=4;
 char flash_buffer[flash_word_size];

  char endfoundcount=0;
  unsigned int i=0;
  unsigned int j=0; // counter for flash writes
 
 DISP1("Copying to flash\r\n")
 

  while (endfoundcount<2)
	{
	  char c=UART_getc(CAM_SERIAL);
	  if (c==JPEG_END[endfoundcount]) endfoundcount+=1; else endfoundcount=0; // should progress through end template
	  flash_buffer[j++]=c;
	  if (j==flash_word_size) {j=0;FlashProgram(flash_buffer,picture_storage+i,flash_word_size);i+=flash_word_size;}
	//  if (endfoundcount==2) {DISP3(".",i,"<");DISP3("",c,">");}
	}


//for (i=0;i<jpegsize;i+=flash_word_size)
 //   {  
//	DISP3("i = ",i,"\r\n");
//	for (j=0;j<flash_word_size;j++)
//	    {
//	    DISP3("j = ",j,"\r\n");
//		if ((i+j)<jpegsize) {flash_buffer[j]=UART_getc(CAM_SERIAL);} else {flash_buffer[j]=0;}
	    //flash_buffer[j]=0x37;
//	    }
//	DISP3("writing : ",picture_storage+i,"\r\n")
//	FlashProgram(flash_buffer,picture_storage+i,flash_word_size);
//	DISP3("set to ",picture_storage[i],"\r\n");
//    }

DISP1("Copy complete \r\n");

  unsigned int picturelength=i+j; //jpegsize; //i+j;

	if (j>0)
	   {
	     while (j<flash_word_size) {flash_buffer[j++]=0;}
		 FlashProgram(flash_buffer,picture_storage+i,flash_word_size); // do final one (excess bytes from previous)
	   }

  DISP3("Last four : ",picture_storage[picturelength-3],",");DISP3(" ",picture_storage[picturelength-2],",");
  DISP3(" ",picture_storage[picturelength-1],",");DISP3(" ",picture_storage[picturelength],"\n\r");

  DISP3("ENDFOUNDCOUNT = ",endfoundcount,"\n\r")
  DISP3("Downloaded ",picturelength," bytes of image data\n\r")
 return picturelength;	
 }

  void send_packet(cmd,value)
    {
    bool packet_not_sent=true;
 //   while (packet_not_sent)
 //      {
    //	UART_putc(DEBUG_SERIAL,'X');DISP3("'",cmd,"'");
    	UART_putc(DEBUG_SERIAL,cmd);
        writeword4(DEBUG_SERIAL,value);
   //     Delay_ms(100); // wait for response
      //  char c=UART_getc(DEBUG_SERIAL);UART_putc(DEBUG_SERIAL,'*');UART_putc(DEBUG_SERIAL,c);
     //   while (c!='A') {c=UART_getc(DEBUG_SERIAL);UART_putc(DEBUG_SERIAL,'!');UART_putc(DEBUG_SERIAL,c);}
    //    if (UART_getc(DEBUG_SERIAL)=='C' && UART_getc(DEBUG_SERIAL)=='K' && UART_getc(DEBUG_SERIAL)==cmd) packet_not_sent=false;
  //  if (packet_not_sent) DISP1("Ack not received\r\n") else DISP1("Ack received")
    //   }
     }

 void ack(unsigned char cmd)
  {
    UART_putc(DEBUG_SERIAL,'A');
    UART_putc(DEBUG_SERIAL,'C');
    UART_putc(DEBUG_SERIAL,'K');
    UART_putc(DEBUG_SERIAL,cmd); 	
  }

 unsigned int receive_packet(unsigned char cmd)
    {
    char c=UART_getc(DEBUG_SERIAL);
    while (c!=cmd) {c=UART_getc(DEBUG_SERIAL);DISP(c);}
    unsigned int data=UART_getw4(DEBUG_SERIAL);
    ack(cmd);
    return data;
    }

int main(void)
{  
  Board_init(); // start the board
  setupwatchdoginterrupt();
 
  UART_init(DEBUG_SERIAL, 115200);   DISP1("\n\n\r    Satellite UART Camera test.\n\r")
  UART_init(CAM_SERIAL, 115200);   

 // start the console, print bootup message (below)

char *picture_storage;

while (1)
 {
   DISP1("Ready for command:"); // ':' is command ready
   char cmd=UART_getc(DEBUG_SERIAL);
   char *ad;unsigned int len;
   switch (cmd)
    {
    case '#':
        // take a picture and store at given address

      	//UART_puts(DEBUG_SERIAL," Take Picture Cmd received.\r\n");

   		picture_storage=UART_getw4(DEBUG_SERIAL);  ///=0x10000; // store it here
        ack('#');
    //  	DISP1(" Take Picture Cmd received.\r\n");
 
   		unsigned int picturelength=take_picture(picture_storage);
   	
   	//	DISP3("Picture length = ",picturelength,"\r\n");
   	//	DISP1("Sending length packet.\n\r");
   		send_packet('$',picturelength);
   	//	UART_putc(DEBUG_SERIAL,'#');writeword4(DEBUG_SERIAL,picturelength); // signal length saved
   	   break;

   	case '@':	 
          ack('~'); // not @ or confuses python
 	      DISP1(" Download memory Cmd received.\r\n")

 		  ad=UART_getw4(DEBUG_SERIAL);
		  len=UART_getw(DEBUG_SERIAL);
		  DISP3("address = ",(int)ad,"\r\n");
		  DISP3("len = ",len,"\r\n");
		  mem_packet_send(ad,len);
		  break;

  default:{}
	//DISP1("Unknown command received\r\n");
   }
 }
}