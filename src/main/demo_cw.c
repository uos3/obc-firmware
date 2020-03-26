#include "../component/radio.h"
#include "../component/cc112x.h"
#include "../component/cc112x_spi.h"
#include "../component/led.h"
#include "../component/cw.h"

#include "../driver/delay.h"
#include "../driver/board.h"
#include "../driver/uart.h"
#include "../driver/watchdog_ext.h"

#include <stdio.h>


#define UART_INTERFACE UART_DEBUG_4
static uint8_t buffer[17] = "UOS3 UOS3 UOS3 k\0";
static uint32_t buffer_length = 16;
static radio_config_t radio_transmitter =
{
  145.5,
  10,    //From -11 to 15
  CC112X_FSK_SYMBOLRATE_50,  //Irrelevant to morse
};


//TODO check register settings (they work)
//Note power amplifier is enabled by the tx management chip not the PA_POWER trace which does nothing on the modern TOBC

int main(void)
{
  char output[100];
  LED_set(LED_B, true);
  Board_init();
  watchdog_update = 0xFF;           //set the watchdog control variable
  enable_watchdog_kick();

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nCW Radio Demo\r\n");

  sprintf(output,"Freq: %.3fMHz, Power: %+.1fdBmW\r\n", radio_transmitter.frequency, radio_transmitter.power);
  UART_puts(UART_INTERFACE, output);

  while(1)
  {
    watchdog_update = 0xFF;           //set the watchdog control variable
    sprintf(output,"Sending Beacon :\"%s\"\r\n", buffer);
    UART_puts(UART_INTERFACE, output);
    Packet_cw_transmit_buffer(buffer, buffer_length, &radio_transmitter, &cw_tone_on, &cw_tone_off);
    Delay_ms(5000);
    UART_puts(UART_INTERFACE, "Sent.\r\n");

  }
}
