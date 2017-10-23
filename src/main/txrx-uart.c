/* firmware.h contains all relevant headers */
#include "../firmware.h"
#include "txrx-uart.h"

#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* A very simple example that blinks the on-board LED. */

//#define UART UART_PC104_HEADER
#define UART UART_CAM_HEADER

void cw_tone_option(void);
void tx_packets_option(void);
uint8_t ui_set_sym_dev(uint8_t radio_id);
uint8_t ui_set_freq_power(uint8_t radio_id);
uint8_t ui_set_power(uint8_t radio_id);
uint8_t ui_set_freq(uint8_t radio_id);
char wait_for_response_char(void);
char peek_for_response_char(void);
void rx_packets_option(void);
uint32_t ui_set_packet_len(uint32_t max_len, uint32_t min_len);
uint16_t ui_get_packet_wait(void);
void tx_packets_pwrsweep_option(void);
void rx_packets_stats_option(void);

uint16_t wait_for_response_ln(void);
double current_pwr = -500;
uint8_t current_pwr_reg = 0;
double current_freq = 0;

#define UART_BUFF_LEN 100
char uart_in_buff[UART_BUFF_LEN] = {0};
char uart_out_buff[UART_BUFF_LEN] = {0};


//void SysTickIntHandler(void)
//{
//    WDT_kick();
//}


int main(void)
{
   Board_init();
   WDT_kick();
   
   SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
   //IntMasterEnable();
   //SysTickIntEnable();
   //SysTickEnable();


   UART_init(UART, 500000);
   
   SPI_init(SPI_RADIO_TX);
   SPI_init(SPI_RADIO_RX);
   
   while(1){
   
      UART_puts(UART, "Welcome to the radio test program\n\n");
      UART_puts(UART, "1) CW tone\n");
      UART_puts(UART, "2) TX FSK packets\n");
      UART_puts(UART, "3) TX FSK packets (power sweep)\n");
      UART_puts(UART, "4) RX FSK packets (stats only)\n");
      UART_puts(UART, "5) RX FSK packets (logtail)\n");
      
      WDT_kick();
      
      char res = wait_for_response_char();
      
      UART_putc(UART, res);
      UART_putc(UART, '\n');
      
      switch (res){
         case '1':
            cw_tone_option();
            break;
         case '2':
            tx_packets_option();
            break;
         case '3':
            tx_packets_pwrsweep_option();
            break;
         case '4':
            rx_packets_stats_option();
            break;
         case '5':
            rx_packets_option();
            break;
         default:
            break;
      }
      
      UART_puts(UART, "Next...\n");
      
   }
      
   //// wait for WDT reset
   //while(1){};

}

char peek_for_response_char(void){
   
   if(!(UART_charsAvail(UART) == 0))   
      return UART_getc(UART);
   else
      return 0;
}

char wait_for_response_char(void){
   
   while(UART_charsAvail(UART) == 0){};
   
   return UART_getc(UART);
}

uint16_t wait_for_response_ln(void){
   
   uint16_t p = 0;
   uint8_t ln = 0;
   
   while((p < (UART_BUFF_LEN-1)) && (ln == 0)){
      while(UART_charsAvail(UART) == 0){};
      uart_in_buff[p] = UART_getc(UART);
      if ((uart_in_buff[p] == '\n') || (uart_in_buff[p] == '\r'))
         ln = 1;
      else
         p++;
   }
   uart_in_buff[p] = '\0';
   return p;   
}

uint8_t ui_set_sym_dev(uint8_t radio_id){
   uint32_t symrate, deviation;
   uint8_t r;
      
   
   
   UART_puts(UART, "Enter symbol rate (/sec, default: 1000 /sec): ");
   wait_for_response_ln();
   symrate = (uint32_t)atoi(uart_in_buff);
   
   if (symrate == 0)
      symrate = 1000;   
   
   UART_puts(UART, "\nEnter deviation (Hz, default: 2000 Hz): ");
   wait_for_response_ln();
   deviation = (uint32_t)atoi(uart_in_buff);
   
   if (deviation == 0)
      deviation = 2000;
   
   r = radio_set_fsk_param(radio_id, &symrate, &deviation);
   snprintf(uart_out_buff, UART_BUFF_LEN, "%li, %li\n", symrate, deviation);
   if (r){
      UART_puts(UART, "\nError in symbol rate, deviation settings entered: ");
      UART_puts(UART, uart_out_buff);
      return 1;
   }
   UART_puts(UART, "\nSetting symbol rate, deviation settings to ");
   UART_puts(UART, uart_out_buff);
   
   return 0;
}

uint8_t ui_set_freq_power(uint8_t radio_id){
   
   if(ui_set_freq(radio_id))
      return 1;
   
   if(ui_set_power(radio_id))
      return 1;   
   
   return 0;
}
uint8_t ui_set_power(uint8_t radio_id){
   double pwr;
   uint8_t r;
   uint8_t pwr_reg=0;
   
   UART_puts(UART, "Enter power (dBm): ");
   
   //res = wait_for_response_ln();
   wait_for_response_ln();
   pwr = atof(uart_in_buff);
   r = radio_set_pwr_f(radio_id, &pwr, &pwr_reg);
   snprintf(uart_out_buff, UART_BUFF_LEN, "%2.1f dBm (reg = %i)\n", pwr, pwr_reg);
   if (r){
      UART_puts(UART, "Error in power entered: ");
      UART_puts(UART, uart_out_buff);
      return 1;
   }
   UART_puts(UART, "Setting power to ");
   UART_puts(UART, uart_out_buff);
   current_pwr = pwr;
   current_pwr_reg = pwr_reg;
   return 0;
}
uint8_t ui_set_freq(uint8_t radio_id){
   double freq;
   uint8_t r;

   
   UART_puts(UART, "Enter frequency (MHz): ");
   //uint16_t res = wait_for_response_ln();
   wait_for_response_ln();
   freq = atof(uart_in_buff);
   r = radio_set_freq_f(radio_id, &freq);
   snprintf(uart_out_buff, UART_BUFF_LEN, "%3.3f MHz\n", freq);
   if (r){
      UART_puts(UART, "Error in frequency entered: ");
      UART_puts(UART, uart_out_buff);
      return 1;
   }
   current_freq = freq;
   UART_puts(UART, "Setting frequency to ");
   UART_puts(UART, uart_out_buff);
   
  
   return 0;
}

uint32_t ui_set_packet_len(uint32_t max_len, uint32_t min_len){
   UART_puts(UART, "\nEnter packet length (default: 20 bytes): ");
   wait_for_response_ln();
   int32_t len = atoi(uart_in_buff);
   if (len == 0)
      len = 20;
   snprintf(uart_out_buff, UART_BUFF_LEN, "%li bytes\n", len);
   if ((len > max_len) || (len < min_len)){
      UART_puts(UART, "Error in length entered: ");
      UART_puts(UART, uart_out_buff);
      return 0;
   }
   UART_puts(UART, "Setting packet length to ");
   UART_puts(UART, uart_out_buff);
   return (uint32_t)len;
}

uint16_t ui_get_packet_wait(void){
   UART_puts(UART, "\nEnter packet wait period (default: 300 ms): ");
   wait_for_response_ln();
   int32_t p = atoi(uart_in_buff);
   if (p <= 0)
      p = 300;
   snprintf(uart_out_buff, UART_BUFF_LEN, "%li ms\n", p);
   UART_puts(UART, "Setting packet wait period to ");
   UART_puts(UART, uart_out_buff);
   return (uint16_t)p;
}

uint8_t ui_set_rxbw(uint8_t radio_id){
   
   uint32_t rxbw;
   uint8_t r;
   
   UART_puts(UART, "Enter RX bandwidth (Hz, default: 8000 Hz): ");
   wait_for_response_ln();
   rxbw = (uint32_t)atoi(uart_in_buff);
   if (rxbw == 0)
      rxbw = 8000;
   
   r = radio_set_rxbw_param(radio_id, &rxbw); //, &symrate);
   snprintf(uart_out_buff, UART_BUFF_LEN, "%li\n", rxbw);
   if (r){
      UART_puts(UART, "\nError in RX bandwidth entered: ");
      UART_puts(UART, uart_out_buff);
      return 1;
   }
   UART_puts(UART, "\nSetting RX bandwidth to ");
   UART_puts(UART, uart_out_buff);
  
   return 0;
}

void rx_packets_stats_option(void){
   
   UART_puts(UART, "\nRX FSK packets stats selected\n");
   
   radio_reset_config(SPI_RADIO_RX, preferredSettings_fsk, sizeof(preferredSettings_fsk)/sizeof(registerSetting_t));
      
   if (ui_set_freq(SPI_RADIO_RX))
      return;
   
   if (ui_set_sym_dev(SPI_RADIO_RX))
      return;
   
   
   if (ui_set_rxbw(SPI_RADIO_RX))
      return;
   
   manualCalibration(SPI_RADIO_RX);
   
   uint8_t num_bytes, marcState;
   uint8_t rxBuff[256]; // can only read the 128 FIFO bytes for now though
   
   SPI_cmdstrobe(SPI_RADIO_RX, CC112X_SRX);
   
   #define CONFIG_CNT 200
   uint16_t config_list[CONFIG_CNT] = {0xFFFF};
   uint32_t last_id_list[CONFIG_CNT] = {0};
   uint32_t err_cnt_list[CONFIG_CNT] = {0};
   uint32_t suc_cnt_list[CONFIG_CNT] = {0};
   uint16_t list_top = 0;
   uint32_t lowest_id = 0xFFFFFFFF;
   uint8_t restart = 0;
   char c;
   
   while(1){
      
      //wait for packet
      while(cc1125_pollGPIO(GPIO0_RADIO_RX) == 0){
         c = peek_for_response_char();
         if (c == 'r')
            restart = 1;
         if (c == 'q')
            return;
      }
		while(cc1125_pollGPIO(GPIO0_RADIO_RX) > 0){
         c = peek_for_response_char();
         if (c == 'r')
            restart = 1;
         if (c == 'q')
            return;
      }
      
      if (restart){
         restart = 0;
         lowest_id = 0xFFFFFFFF;
         list_top = 0;
      }
         
         
           
           
      //move to the cc1125 file      
      
      // see how long the packet is
      cc112xSpiReadReg(SPI_RADIO_RX, CC112X_NUM_RXBYTES, &num_bytes);
      if(num_bytes) {

         // Read MARCSTATE to check for RX FIFO error
         cc112xSpiReadReg(SPI_RADIO_RX, CC112X_MARCSTATE, &marcState);
         if((marcState & 0x1F) == 0x11) {  // == RX_FIFO_ERROR?
         UART_puts(UART, "\nError, FIFO error... :/\n");
         // Flush RX FIFO
         SPI_cmdstrobe(SPI_RADIO_RX, CC112X_SFRX);
         } else {


            cc112xSpiReadRxFifo(SPI_RADIO_RX, rxBuff, num_bytes);

            if (num_bytes > 6){

               uint8_t crc;
               uint16_t cfg;
               uint32_t packetid;

               packetid = rxBuff[2] | (rxBuff[3] << 8) | (rxBuff[4] << 16) | (rxBuff[5] << 24);                 
               cfg = rxBuff[1];
               crc = rxBuff[num_bytes - 1] & 0x80;
               if (crc){
                  
                  if (lowest_id == 0xFFFFFFFF)
                     lowest_id = packetid;
               
                  // search to see if we've already stored this cfg
                  uint16_t i;
                  uint8_t found = 0;
                  for (i = 0; i < list_top; i++){
                     if (config_list[i] == cfg){
                        found = 1;
                        break;
                     }
                  }
                  if (found == 0){
                     if (!(list_top >= CONFIG_CNT)){
                        i = list_top;
                        list_top++;
                        config_list[i] = cfg;
                        last_id_list[i] = lowest_id;
                        err_cnt_list[i] = 0;
                        suc_cnt_list[i] = 0;
                        found = 1;
                     }
                  }
                  
                  // update cfg
                  if (found && (packetid > lowest_id)){
                     uint32_t missing = packetid - last_id_list[i] - 1;
                     last_id_list[i] = packetid;
                     err_cnt_list[i] += missing;
                     if (crc)
                        suc_cnt_list[i] += 1;
                     else
                        err_cnt_list[i] += 1;
                     snprintf(uart_out_buff, UART_BUFF_LEN, "id: %li, cfg: %i, crc?: %i ", packetid,cfg,crc);
                     UART_puts(UART, uart_out_buff);
                     snprintf(uart_out_buff, UART_BUFF_LEN, "tot pass: %li, tot fail: %li\n", suc_cnt_list[i], err_cnt_list[i]);
                     UART_puts(UART, uart_out_buff);
                  }
               }
            }
         }
      }
      else
         UART_puts(UART, "\nError, Pin asserted, but no bytes in FIFO! :/\n");
      
      SPI_cmdstrobe(SPI_RADIO_RX, CC112X_SRX);
      
   }   
}

void rx_packets_option(void){
   
   UART_puts(UART, "\nRX FSK packets selected\n");
   
   radio_reset_config(SPI_RADIO_RX, preferredSettings_fsk, sizeof(preferredSettings_fsk)/sizeof(registerSetting_t));
      
   if (ui_set_freq(SPI_RADIO_RX))
      return;
   
   if (ui_set_sym_dev(SPI_RADIO_RX))
      return;
  
   
   if (ui_set_rxbw(SPI_RADIO_RX))
      return;
   
   manualCalibration(SPI_RADIO_RX);
   
   uint8_t num_bytes, marcState;
   uint8_t rxBuff[256]; // can only read the 128 FIFO bytes for now though
   uint8_t i;
   
   SPI_cmdstrobe(SPI_RADIO_RX, CC112X_SRX);
   
   while(1){
      
      //wait for packet
      while(cc1125_pollGPIO(GPIO0_RADIO_RX) == 0){};
		while(cc1125_pollGPIO(GPIO0_RADIO_RX) > 0){};
           
           
      //move to the cc1125 file      
      
      // see how long the packet is
      cc112xSpiReadReg(SPI_RADIO_RX, CC112X_NUM_RXBYTES, &num_bytes);
      if(num_bytes) {

          // Read MARCSTATE to check for RX FIFO error
          cc112xSpiReadReg(SPI_RADIO_RX, CC112X_MARCSTATE, &marcState);
          if((marcState & 0x1F) == 0x11) {  // == RX_FIFO_ERROR?
              UART_puts(UART, "\nError, FIFO error... :/\n");
              // Flush RX FIFO
              SPI_cmdstrobe(SPI_RADIO_RX, CC112X_SFRX);
          } else {


              cc112xSpiReadRxFifo(SPI_RADIO_RX, rxBuff, num_bytes);

              // Check CRC ok (CRC_OK: bit7 in second status byte)
              // This assumes status bytes are appended in RX_FIFO
              // (PKT_CFG1.APPEND_STATUS = 1)
              // If CRC is disabled the CRC_OK field will read 1
              if(rxBuff[num_bytes - 1] & 0x80) {
                  snprintf(uart_out_buff, UART_BUFF_LEN, "Rx OK   (%d bytes) - ", num_bytes);

              }else{
                  snprintf(uart_out_buff, UART_BUFF_LEN, "CRC ERR (%d bytes) - ", num_bytes);
              }
              UART_puts(UART, uart_out_buff);
              
              for (i = 0; i < num_bytes; i++){
                 snprintf(uart_out_buff, UART_BUFF_LEN, "%02x ", rxBuff[i]);
                 UART_puts(UART, uart_out_buff);
              }
              UART_puts(UART, "    ");
              for (i = 0; i < num_bytes; i++){
                 if (rxBuff[i] < 32)
                    UART_putc(UART, ' ');
                 else
                    UART_putc(UART, (char)rxBuff[i]);
              }
              
              UART_putc(UART, '\n');
          }
      }
      else
         UART_puts(UART, "\nError, Pin asserted, but no bytes in FIFO! :/\n");
      
      SPI_cmdstrobe(SPI_RADIO_RX, CC112X_SRX);
      
   }
   
}

void tx_packets_option(void){
  
   
   UART_puts(UART, "\nTX FSK packets selected\n");
   
   radio_reset_config(SPI_RADIO_TX, preferredSettings_fsk, sizeof(preferredSettings_fsk)/sizeof(registerSetting_t));
      
   if (ui_set_freq_power(SPI_RADIO_TX))
      return;
   
   if (ui_set_sym_dev(SPI_RADIO_TX))
      return;
   
   //while(wait_for_response_char() != 'q'){};
   uint32_t len = ui_set_packet_len(256-3, 6);
   if (len < 1)
      return;
   
   uint16_t wait = ui_get_packet_wait();
      
   uint32_t cnt=0;
   char c;
   
    UART_puts(UART, "w/s - Increase/decrease power in ~0.5dB increments\n");
    UART_puts(UART, "a/d - Increase/decrease frequency in 500Hz increments\n\n");
    while(1){
      manualCalibration(SPI_RADIO_TX);
      
      uint8_t buff[256];
      uint8_t i;
      cnt++;
      buff[0] = (uint8_t)len;
      buff[1] = current_pwr_reg;
      buff[2] = (uint8_t)(cnt & 0xFF);
      buff[3] = (uint8_t)((cnt>>8) & 0xFF);
      buff[4] = (uint8_t)((cnt>>16) & 0xFF);
      buff[5] = (uint8_t)((cnt>>24) & 0xFF);
      for (i = 6; i < len+1; i++)
         buff[i] = i;
      
     
      cc112xSpiWriteTxFifo(SPI_RADIO_TX, buff, (uint8_t)(len+1));

      SPI_cmdstrobe(SPI_RADIO_TX, CC112X_STX);
      uint32_t w1,w2;
      

         
      // wait for the packet to send
      while( cc1125_pollGPIO(GPIO0_RADIO_TX)) {} ;

      // wait a little
      for(w2 = 0; w2 < wait; w2++){
         for(w1 = 0; w1 < 1000; w1++) {};
      }
      
      c = peek_for_response_char();
      if (c == 'w'){
         SPI_cmdstrobe(SPI_RADIO_TX, CC112X_SIDLE);
         if (radio_set_pwr_reg(SPI_RADIO_TX, current_pwr_reg+1)==0){
            current_pwr_reg++;
            snprintf(uart_out_buff, UART_BUFF_LEN, "Power register set to: %i (~%2.1f dBm)\n", current_pwr_reg, radio_pwr_reg_to_dbm(current_pwr_reg));
            UART_puts(UART, uart_out_buff);            
         }
      }
      if (c == 's'){
         SPI_cmdstrobe(SPI_RADIO_TX, CC112X_SIDLE);
         if (radio_set_pwr_reg(SPI_RADIO_TX, current_pwr_reg-1)==0){
            current_pwr_reg--;
            snprintf(uart_out_buff, UART_BUFF_LEN, "Power register set to: %i (~%2.1f dBm)\n", current_pwr_reg, radio_pwr_reg_to_dbm(current_pwr_reg));
            UART_puts(UART, uart_out_buff);            
         }
      }
      if (c == 'd'){
         SPI_cmdstrobe(SPI_RADIO_TX, CC112X_SIDLE);
         current_freq += 0.0005;
         radio_set_freq_f(SPI_RADIO_TX, &current_freq);
         snprintf(uart_out_buff, UART_BUFF_LEN, "Frequency set to: %3.4f MHz\n", current_freq);
         UART_puts(UART, uart_out_buff);
      }
      if (c == 'a'){
         SPI_cmdstrobe(SPI_RADIO_TX, CC112X_SIDLE);
         current_freq -= 0.0005;
         radio_set_freq_f(SPI_RADIO_TX, &current_freq);
         snprintf(uart_out_buff, UART_BUFF_LEN, "Frequency set to: %3.4f MHz\n", current_freq);
         UART_puts(UART, uart_out_buff);
      }
      
      
      
      if (c == 'q')
         return;
    
   }
   

   return;
}

void tx_packets_pwrsweep_option(void){
  
   
   UART_puts(UART, "\nTX FSK packets power sweep selected\n");
   
   radio_reset_config(SPI_RADIO_TX, preferredSettings_fsk, sizeof(preferredSettings_fsk)/sizeof(registerSetting_t));
      
   if (ui_set_freq(SPI_RADIO_TX))
      return;
   
   if (ui_set_sym_dev(SPI_RADIO_TX))
      return;
   
   uint32_t len = ui_set_packet_len(256-3, 6);
   if (len < 1)
      return;
   
   uint16_t wait = ui_get_packet_wait();
      
   uint32_t cnt=1;
   uint8_t pwr_reg = 3;
   uint8_t writebyte;
   
    while(1){
       
      for (pwr_reg = 3; pwr_reg < 0x3F; pwr_reg++){
         writebyte = (uint8_t)((1<<6) | pwr_reg);
         cc112xSpiWriteReg(SPI_RADIO_TX, CC112X_PA_CFG2, &writebyte);
          
         manualCalibration(SPI_RADIO_TX);
         
         uint8_t buff[256];
         uint8_t i;
         
         buff[0] = (uint8_t)len;
         buff[1] = pwr_reg;
         buff[2] = (uint8_t)(cnt & 0xFF);
         buff[3] = (uint8_t)((cnt>>8) & 0xFF);
         buff[4] = (uint8_t)((cnt>>16) & 0xFF);
         buff[5] = (uint8_t)((cnt>>24) & 0xFF);
         for (i = 6; i < len+1; i++)
            buff[i] = i;
         
        
         cc112xSpiWriteTxFifo(SPI_RADIO_TX, buff, (uint8_t)(len+1));

         SPI_cmdstrobe(SPI_RADIO_TX, CC112X_STX);
         uint32_t w1,w2;
         

            
         // wait for the packet to send
         while( cc1125_pollGPIO(GPIO0_RADIO_TX)) {} ;

         // wait a little
         for(w2 = 0; w2 < wait; w2++){
            for(w1 = 0; w1 < 1000; w1++) {};
         }
      }
      cnt++;
    
   }
   

   return;
}

void cw_tone_option(void){
   
 
   UART_puts(UART, "\nCW tone selected\n");
   
   radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));
   manualCalibration(SPI_RADIO_TX);
   
   if (ui_set_freq_power(SPI_RADIO_TX))
      return;
   
   // turn on radio   
   UART_puts(UART, "CW tone on. Press q to quit\n");   
   SPI_cmdstrobe(SPI_RADIO_TX, CC112X_STX);
   
   while(wait_for_response_char() != 'q'){};
   radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));

   
}