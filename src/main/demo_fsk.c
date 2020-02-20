
#include "../board/radio.h"
#include "../board/watchdog_ext.h"

#define useMSK 0

// sample packet
uint8_t sample_packet[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x01, 0x0F, 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'}

// static uint8_t buffer[17] = "UOS3 UOS3 UOS3 k\0";
// static uint32_t buffer_length = 16;
static radio_config_t radio_transmitter =
    {
        145.5, //Frequency, currently this is not used
        9.0,   //Power From -11 to 15, this is not actually the dbm it will come out at
        CC112X_FSK_SYMBOLRATE_600,
};
//Interestingly as you write higher powers to the device, beyond about 9dB, power output actually drops

#define PACKET_LENGTH_32x32 128 // 1024/8

uint8_t packet_test_32x32[PACKET_LENGTH_32x32];
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
  char output[100];
  Board_init();
  watchdog_update = 0xFF;           //set the watchdog control variable 
  enable_watchdog_kick();

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nCW Radio fsk Demo\r\n");

  sprintf(output,"Freq: %.3fMHz, Power: %+.1fdBmW\r\n", radio_transmitter.frequency, radio_transmitter.power);
  UART_puts(UART_INTERFACE, output);
  
  uint32_t i;

  /* Populate packet */
  for(i=0; i<PACKET_LENGTH_32x32; i++)
  {
    packet_test_32x32[i] = (uint8_t)(Random(255));
  }
  uint32_t buffer_length = PACKET_LENGTH_32x32;

while(1)
  {
    watchdog_update = 0xFF;           //set the watchdog control variable 
    sprintf(output,"Sending Beacon :");    //\"%s\"\r\n", buffer);
    UART_puts(UART_INTERFACE, output);
    if (!useMSK)  {
      Radio_tx_fsk(&radio_transmitter, packet_test_32x32, buffer_length, &transmit_complete);
    }
    else
    {
      //Radio_tx_msk
    }
    //This delay has to be long enough to prevent the tobc from interupting the transmission 
    Delay_ms(10000);
    UART_puts(UART_INTERFACE, "Sent.\r\n");

  }
}
