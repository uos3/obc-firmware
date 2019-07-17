/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>
#include <stdlib.h>

//#define PAGE_LENGTH 5000 //Buffer_slot_size is 3392bytes, do changed the page size!
#define PAGE_LENGTH 106

//uint32_t image_length, number_of_pages;     //variables to store the length of the received image and number of pages received
//uint8_t *total_data;                        //variable to store the picture
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

static uint32_t UART_getw4(uint8_t serial)
{
  char c1=UART_getc(serial),c2=UART_getc(serial),c3=UART_getc(serial),c4=UART_getc(serial);
  return (uint32_t)((c1 << 24) | (c2 << 16) | (c3 << 8) | (c4 << 0)); //shifting values of read char/hex numbers to create one 32bit number
}

int main(void)
{
  /*char output[100];
  /char time[100];
  uint64_t timestamp;*/
  Board_init();
  RTC_init();

  UART_init(UART_GNSS,115200);
  UART_puts(UART_GNSS, "Camera Demo\r\n");
  UART_init(UART_CAMERA, 38400);
  int stop = 1;
  while(stop--)
  { 
    //image_length = 0;
    //number_of_pages = 0;
    /*LED_on(LED_B);
    UART_puts(UART_GNSS, "\r\nCapturing..\r\n");
    RTC_getTime_ms(&timestamp);
    sprintf(time, "Time is %d\r\n",timestamp);
    UART_puts(UART_GNSS, time);

    if(Camera_capture(PAGE_LENGTH, &image_length, &number_of_pages, total_data))
    {
        sprintf(output, "Number of received pages is %d; image length is %d \r\n", number_of_pages, image_length);
        UART_puts(UART_GNSS,output);
        char output[20];
        int print = 0;
        while (print<=(image_length-1))
        {
          sprintf(output, "0x%02"PRIx8" ", total_data[print]);
          UART_puts(UART_GNSS, output);
          print++;
        }
    }
    else
    {
        sprintf(output, "FAIL (%"PRIu32" Bytes) -- (Number of received pages - %"PRIu32")\r\n", image_length, number_of_pages);
        UART_puts(UART_GNSS, output);
    }
    stop--;*/
    //variables
    int index_rst=0, index_res=0, index_takepic=0, index_readsize=0, index_readdata=0, index_readdata2 =0;
    int rst = 0, res = 0, pic = 0, size=0, dat =0, dat2 = 0, dat3=0;
    char reset_resp[20], res_resp[20], pic_res[20], size_resp[20]={0}, read_resp[20]={0}, read_resp2[20];
    char *output = malloc(100*sizeof(char));
    char *output2 = malloc(100*sizeof(char));
    char *output3 = malloc(100*sizeof(char));
    char *output4 = malloc(100*sizeof(char));
    char *output5 = malloc(100*sizeof(char));
    char *output6 = malloc(100*sizeof(char));
    char *output7 = malloc(100*sizeof(char));
    uint32_t jpegsize;
    UART_puts(UART_GNSS, "\r\nSTART: \r\n");
    //reset command
    UART_putb(UART_CAMERA, LK_RESET, sizeof(LK_RESET));
    uint64_t start_timestamp;
    RTC_getTime_ms(&start_timestamp);
  while(!RTC_timerElapsed_ms(start_timestamp, 3000))
  { 
    if(UART_charsAvail(UART_CAMERA)){
    if(UART_getc_nonblocking(UART_CAMERA, &reset_resp[rst]))
    {
      if(reset_resp[rst] == LK_RESET_RE[index_rst])
      {
        index_rst++;
        if(index_rst == sizeof(LK_RESET_RE))
        {
          break;;
        }
      }
      else
      {
        index_rst = 0;
      }
    }
    rst++;
    }
  }
    /*
    Delay_ms(25);
    while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&reset_resp[rst]);
      if(reset_resp[rst] == LK_RESET_RE[index_rst]){
        index_rst++;
      }else{
        index_rst = 0;
      }
      rst++;
    }*/
    if(index_rst == sizeof(LK_RESET_RE)){UART_puts(UART_GNSS, "\r\nReset response matched\r\n");}
      else{UART_puts(UART_GNSS, "\r\nReset response not matched\r\n");}
    UART_putb(UART_GNSS, reset_resp, sizeof(LK_RESET_RE));
    for(int i = 0;i<sizeof(LK_RESET_RE);i++){
      sprintf(output, "0x%02"PRIx8" ", reset_resp[i]);
      UART_puts(UART_GNSS, output);
    }
    Delay_ms(3000);

    //take picture command
    
    UART_putb(UART_CAMERA, LK_PICTURE, sizeof(LK_PICTURE));
    Delay_ms(200);
    while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&pic_res[pic]);
      if(pic_res[pic] == LK_PICTURE_RE[index_takepic]){
        index_takepic++;
      }else{
        index_takepic=0;
      }
      pic++;
    }
    if(index_takepic == sizeof(LK_PICTURE_RE)){UART_puts(UART_GNSS, "\r\nPicture Resp matched\r\n");}
    else{UART_puts(UART_GNSS, "\r\nPicture response not matched\r\n");}
    UART_putb(UART_GNSS, pic_res, sizeof(LK_PICTURE_RE));
    for(int i = 0;i<sizeof(LK_PICTURE_RE);i++){
      sprintf(output2, "0x%02"PRIx8" ", pic_res[i]);
      UART_puts(UART_GNSS, output2);
    }
    //read picture size
    UART_putb(UART_CAMERA, LK_JPEGSIZE, sizeof(LK_JPEGSIZE));
    Delay_ms(200);
    while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&size_resp[size]);
      if(size_resp[size]==LK_JPEGSIZE_RE[index_readsize]){
        index_readsize++;
      }
      else{
        index_readsize=0;
      }
      size++;
      if(index_readsize == sizeof(LK_JPEGSIZE_RE)){
        break;
      }
    }
    if(index_readsize == sizeof(LK_JPEGSIZE_RE)){
      UART_puts(UART_GNSS, "\r\nPicture Size Resp matched\r\n");
    }else{UART_puts(UART_GNSS, "\r\nPicture size response not matched\r\n");}
    jpegsize = UART_getw4(UART_CAMERA);
    UART_putb(UART_GNSS, size_resp, sizeof(size_resp));
    for(int i = 0; i<sizeof(LK_JPEGSIZE_RE);i++){
      sprintf(output3, "0x%02"PRIx8" ",size_resp[i]);
      UART_puts(UART_GNSS, output3);
    }
  UART_puts(UART_GNSS,"\n\r");
  sprintf(output4, "%u ", jpegsize);
  UART_puts(UART_GNSS, output4);


  UART_puts(UART_GNSS,"Before reading picture data");
  //reading the picture
  uint8_t pagesize =  208;
  bool endflag = false;
  uint8_t data_buffer[208] = {9};
  //LK_READPICTURE[10] = (char)((jpegsize >> 24) & 0xFF);
  //LK_READPICTURE[11] = (char)((jpegsize >> 16) & 0xFF);
  //LK_READPICTURE[12] = (char)((jpegsize >> 8) & 0xFF);
  //LK_READPICTURE[13] = (char)(jpegsize & 0xFF);
  LK_READPICTURE[13]=pagesize;
  LK_READPICTURE[14]= 0x00;
  LK_READPICTURE[15]=0x0a;
  //while(!endflag){
  //send command and wait for the response
    UART_putb(UART_CAMERA,LK_READPICTURE,sizeof(LK_READPICTURE));
    Delay_ms(25);
    while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&read_resp[dat]);
      if(read_resp[dat]==LK_READPICTURE_RE[index_readdata]){
        index_readdata++;
      }else{
        index_readdata = 0;
      }
      dat++;
      if(index_readdata==sizeof(LK_READPICTURE_RE)){break;}
    }if(index_readdata == sizeof(LK_READPICTURE_RE)){
      UART_puts(UART_GNSS, "\r\nData Resp matched\r\n");
    }else{UART_puts(UART_GNSS, "\r\nData response not matched\r\n");}
  
    UART_putb(UART_GNSS, read_resp, sizeof(read_resp));
    for(int i = 0; i<sizeof(LK_READPICTURE_RE);i++){
      sprintf(output5, "0x%02"PRIx8" ",read_resp[i]);
      UART_puts(UART_GNSS, output5);
    }
  Delay_ms(10);
  UART_puts(UART_GNSS,"\n\r");
  while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&data_buffer[dat2]);
      dat2++;
  }
  /*int x =2;
  while(x--){
  for(int k = 0; k<pagesize;k++){
    data_buffer[k]=UART_getc(UART_CAMERA);
  }*/
  for(int i =0;i<pagesize;i++){
    sprintf(output6, "0x%02"PRIx8" ",data_buffer[i]);
      UART_puts(UART_GNSS, output6);
  }
  UART_putb(UART_CAMERA,LK_READPICTURE,sizeof(LK_READPICTURE));
  Delay_ms(25);
  dat2=0;
  UART_puts(UART_GNSS,"\n\r");
  while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&data_buffer[dat2]);
      dat2++;
  }
  for(int i =0;i<pagesize;i++){
    sprintf(output7, "0x%02"PRIx8" ",data_buffer[i]);
      UART_puts(UART_GNSS, output7);
  }
  UART_putb(UART_CAMERA,LK_READPICTURE,sizeof(LK_READPICTURE));
  Delay_ms(25);
  dat2=0;
  UART_puts(UART_GNSS,"\n\r");
  while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&data_buffer[dat2]);
      dat2++;
  }
  for(int i =0;i<pagesize;i++){
    sprintf(output7, "0x%02"PRIx8" ",data_buffer[i]);
      UART_puts(UART_GNSS, output7);
  }
  UART_putb(UART_CAMERA,LK_READPICTURE,sizeof(LK_READPICTURE));
  Delay_ms(25);
  dat2=0;
  UART_puts(UART_GNSS,"\n\r");
  while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&data_buffer[dat2]);
      dat2++;
  }
  for(int i =0;i<pagesize;i++){
    sprintf(output7, "0x%02"PRIx8" ",data_buffer[i]);
      UART_puts(UART_GNSS, output7);
  }
  UART_putb(UART_CAMERA,LK_READPICTURE,sizeof(LK_READPICTURE));
  Delay_ms(25);
  dat2=0;
  UART_puts(UART_GNSS,"\n\r");
  while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&data_buffer[dat2]);
      dat2++;
  }
  for(int i =0;i<pagesize;i++){
    sprintf(output7, "0x%02"PRIx8" ",data_buffer[i]);
      UART_puts(UART_GNSS, output7);
  }
/*



    Delay_ms(10);
    int i =0;
    while(i<pagesize && !endflag){
      data_buffer[i]=UART_getc(UART_CAMERA);
      if((data_buffer[i-1]==0xFF)&&(data_buffer[i]==0xD9)){
        endflag = true;
      }
      i++
    }
    UART_putb(UART_GNSS, data_buffer);

    Delay_ms(10);
    while(UART_charsAvail(UART_CAMERA)){
      UART_getc_nonblocking(UART_CAMERA,&read_resp2[dat2]);
      if(read_resp2[dat2]==LK_READPICTURE_RE[index_readdata2]){
        index_readdata2++;
      }else{
        index_readdata2 = 0;
      }
      dat2++;
      if(index_readdata2==sizeof(LK_READPICTURE_RE)){break;}
    }
  }*/


    free(output2);
    free(output);
    free(output3);
    free(output4);
    free(output5);
    free(output6);
    free(output7);
    LED_off(LED_B);
    Delay_ms(1000);
  }
}
/* 
void image_length_add(uint8_t *data, uint32_t data_length, uint32_t endfoundcount)
{
    char output[60];
    //char output2[60];
    //test
    //DOES NOT WORK, CAUSES DRIVER TO FAIL
    //char data_out[data_length];
    (void) data;
    //sprintf(output2, "endfoundcount is: %d\r\n", endfoundcount);
    //UART_puts(UART_GNSS, output2);
    image_length += data_length;
    //if (endfoundcount == 0){
    //test
    /*for(uint32_t k=0; k<data_length; k++){
        total_data[next_frame] = data[k];
        //UART_puts(UART_GNSS, "data\r\n");
        //sprintf(data_out,"%02" PRIx8 ", ", data[k] );
        //UART_puts(UART_GNSS, data_out);
        next_frame++;
    //}
    }
    total_data[endfoundcount] = *data;  //endfoundcount is the value of counter and show the index of the data line/frame/page received
    sprintf(output, "0x%02"PRIx8", 0x%02"PRIx8"  PRIu32"B).. 0x%02"PRIx8", "0x%02"PRIx8"\r\n",
     data[0], data[1],
     data_length,
     data[data_length-2], data[data_length-1]
    );
    UART_puts(UART_GNSS, output);
}*/