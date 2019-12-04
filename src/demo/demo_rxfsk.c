#include "../firmware.h"
#define UART_INTERFACE UART_DEBUG_4
static radio_config_t radio_receiver = 
{
  145.5,  //Frequency, currently this is not used
  9.0,   //Power From -11 to 15, irrelevant for receiver
  CC112X_FSK_SYMBOLRATE_1200,
};

//GPIO_PB7 connected to Rx GPIO0


#define PACKET_LENGTH_32x32 128 // 1024/8

uint8_t packet_buffer[PACKET_LENGTH_32x32];

void packetReceived(void)
{
  Radio_read_rx_fifo(&packet_buffer);
  for (uint8_t i = 0; i < 128; i++) {
      UART_puts(UART_INTERFACE, (char *)&packet_buffer[i]);
  }
}

//TODO sort out interrupt on PB7
//Check variable packet length setting against packets

//Remember to check deviation is set to same as transmitter in radio.c


int main(void)
{
  char output[100];
  Board_init();
  watchdog_update = 0xFF;           //set the watchdog control variable 
  enable_watchdog_kick();

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nCW Radio FSK Rx Demo\r\n");


  //Ideally set_rising interrupt would be part of enable ewor but it gets akwards with passing rxbufer to it
  Radio_enable_rx_ewor_fsk(radio_receiver, &radio_receiver);
  GPIO_set_risingInterrupt(GPIO_PD7, &packetReceived);


while(1)
  {
    watchdog_update = 0xFF;           //set the watchdog control variable 
    UART_puts(UART_INTERFACE, "Listening");
    Delay_ms(9000);
  }
}
