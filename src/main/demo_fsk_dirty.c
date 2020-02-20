#include "../component/radio.h"
#include "../component/cc112x.h"
#include "../component/cc112x_spi.h"
#include "../component/led.h"
#include "../component/cw.h"

#include "../driver/delay.h"
#include "../driver/board.h"
#include "../driver/uart.h"
#include "../driver/watchdog_ext.h"

#include "../utility/debug.h"

#include <stdio.h>

// sample packet
uint8_t sample_packet[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x01, 0x0F, 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
#define sample_packet_length 31

// static uint8_t buffer[17] = "UOS3 UOS3 UOS3 k\0";
// static uint32_t buffer_length = 16;
static radio_config_t radio_transmitter =
    {
        145.5, //Frequency, currently this is not used
        9.0,   //Power From -11 to 15, this is not actually the dbm it will come out at
        CC112X_FSK_SYMBOLRATE_6000,
};
static void transmit_complete(void);


//TODO check register settings
//Add cfg_msk_params
//And auto preamble /sync word insertion
//And GPIO2 isnt connected to anything so prob should set high impedance
//Need to change buffer content

//0.5kHz deviation for 1 kbps data (symbol) rate from comms gdp


static void transmit_complete(void)  {
  //Increment memory pointer?
  //transmit next packet
}

int main(void)
{
  debug_init();

  debug_print("FSK DIRTY DEMO 02/2020");

  if (!(cc112x_query_partnumber(SPI_RADIO_TX))) {
    debug_print("Part Number Query Failed");
  }
  uint8_t TX_result = cc112xGetRxStatus(SPI_RADIO_TX);
  debug_hex(TX_result, sizeof(uint8_t));

  uint32_t buffer_length = sample_packet_length;

while(1)
  {
    watchdog_update = 0xFF;           //set the watchdog control variable
    debug_print("Sending Beacon");

    //cc112x_read_config(SPI_RADIO_TX, registerlist, sizeof(registerlist));
    //This delay has to be long enough to prevent the tobc from interupting the transmission
    Delay_ms(10000);
    UART_puts(UART_INTERFACE, "Sent.\r\n");

  }
}
