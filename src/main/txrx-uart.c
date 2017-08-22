/* firmware.h contains all relevant headers */
#include "../firmware.h"
#include "txrx-uart.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* A very simple example that blinks the on-board LED. */

#define UART UART_PC104_HEADER

void cw_tone_option(void);

char wait_for_response_char(void);
uint16_t wait_for_response_ln(void);

#define UART_BUFF_LEN 100
char uart_in_buff[UART_BUFF_LEN] = {0};
char uart_out_buff[UART_BUFF_LEN] = {0};

int main(void)
{
   Board_init();
   WDT_kick();


   UART_init(UART, 500000);
   
   SPI_init(SPI_RADIO_TX);
   SPI_init(SPI_RADIO_RX);
   
   UART_puts(UART, "Welcome to the radio test program\n\n");
   UART_puts(UART, "1) CW tone\n");
   UART_puts(UART, "2) TX FSK packets\n");
   UART_puts(UART, "3) TX FSK packets (power sweep)\n");
   UART_puts(UART, "4) RX FSK packets (stats only)\n");
   UART_puts(UART, "5) RX FSK packets (logtail)\n");
   
   char res = wait_for_response_char();
   
   UART_putc(UART, res);
   UART_putc(UART, '\n');
   
   switch (res){
      case '1':
         cw_tone_option();
         break;
      case '2':
      
         break;
      case '3':
      
         break;
      case '4':
      
         break;
      case '5':
      
         break;
      default:
         break;
   }
   
   // wait for WDT reset
   while(1){};
   
   
   
   while(1)
   {
   LED_on(LED_B);

   /* On period */
   Delay_ms(500);
   WDT_kick();

   LED_off(LED_B);

   /* Off period */
   Delay_ms(500);
   WDT_kick();
   }
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

void cw_tone_option(void){
   
   double pwr, freq;
   uint8_t r;
   uint8_t pwr_reg=0;
   
   radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));
   manualCalibration(SPI_RADIO_TX);
   
   UART_puts(UART, "\nCW tone selected\n");
   
   
   UART_puts(UART, "Enter frequency (MHz): ");
   //uint16_t res = wait_for_response_ln();
   wait_for_response_ln();
   freq = atoi(uart_in_buff);
   r = radio_set_freq_f(SPI_RADIO_TX, &freq);
   //snprintf(uart_out_buff, UART_BUFF_LEN, "%3.3f MHz\n", freq);
   snprintf(uart_out_buff, UART_BUFF_LEN, "%d MHz\n", 5);
   if (r){
      UART_puts(UART, "Error in frequency entered: ");
      UART_puts(UART, uart_out_buff);
      return;
   }
   UART_puts(UART, "Setting frequency to ");
   UART_puts(UART, uart_out_buff);
   
   UART_puts(UART, "Enter power (dBm): ");
   
   //res = wait_for_response_ln();
   wait_for_response_ln();
   pwr = atoi(uart_in_buff);
   r = radio_set_pwr_f(SPI_RADIO_TX, &pwr, &pwr_reg);
   //snprintf(uart_out_buff, UART_BUFF_LEN, "%2.1f dBm (reg = %i)\n", pwr, pwr_reg);
   if (r){
      UART_puts(UART, "Error in power entered: ");
      UART_puts(UART, uart_out_buff);
      return;
   }
   UART_puts(UART, "Setting power to ");
   UART_puts(UART, uart_out_buff);
   
   // turn on radio   
   UART_puts(UART, "CW tone on. Press q to quit\n");   
   SPI_cmdstrobe(SPI_RADIO_TX, CC112X_STX);
   
   while(wait_for_response_char() != 'q'){};
   
}