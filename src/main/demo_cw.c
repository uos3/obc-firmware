/* firmware.h contains all relevant headers */
#include "../firmware.h"
//#include "../board/radio.h"
//#include "../cc1125/cc1125.h"
//#include "../cc1125/cc112x_spi.h"
//#include "../board/uos3-proto/board.h"
//
//#include <stdio.h>

// static const registerSetting_t preferredSettings_cw[]= 
// {
//    {CC112X_IOCFG3,         0x59},
//    {CC112X_IOCFG2,         0x13},
//    {CC112X_IOCFG1,         0xB0},
//    {CC112X_IOCFG0,            0x06},
//    {CC112X_SYNC_CFG1,         0x08},
//    {CC112X_DEVIATION_M,       0xB4},
//    {CC112X_MODCFG_DEV_E,      0x0A},
//    {CC112X_DCFILT_CFG,        0x1C},
//    {CC112X_PREAMBLE_CFG1,     0x00},
//    {CC112X_FREQ_IF_CFG,       0x35},
//    {CC112X_IQIC,              0xC6},
//    {CC112X_CHAN_BW,           0x10},
//    {CC112X_MDMCFG1,           0x06},
//    {CC112X_MDMCFG0,           0x05},
//    {CC112X_SYMBOL_RATE2,      0x40},
//    {CC112X_SYMBOL_RATE1,      0x62},
//    {CC112X_SYMBOL_RATE0,      0x4E},
//    {CC112X_AGC_REF,           0x20},
//    {CC112X_AGC_CS_THR,        0x19},
//    {CC112X_AGC_CFG1,          0xA9},
//    {CC112X_AGC_CFG0,          0xCF},
//    {CC112X_FIFO_CFG,          0x00},
//    {CC112X_SETTLING_CFG,      0x03},
//    {CC112X_FS_CFG,            0x1B},
//    {CC112X_PKT_CFG2,          0x05},
//    {CC112X_PKT_CFG1,          0x00},
//    {CC112X_PKT_CFG0,          0x20},
//    {CC112X_PA_CFG2,           0x34},
//    {CC112X_PA_CFG0,           0x7E},
//    {CC112X_IF_MIX_CFG,        0x00},
//    {CC112X_FREQOFF_CFG,       0x22},
//    {CC112X_CFM_DATA_CFG,      0x01},
//    {CC112X_FREQ2,             0x5A},
//    {CC112X_FREQ1,             0xEF},
//    {CC112X_FREQ0,             0xFF},
//    {CC112X_IF_ADC0,           0x05},
//    {CC112X_FS_DIG1,           0x00},
//    {CC112X_FS_DIG0,           0x5F},
//    {CC112X_FS_CAL0,           0x0E},
//    {CC112X_FS_DIVTWO,         0x03},
//    {CC112X_FS_DSM0,           0x33},
//    {CC112X_FS_DVC0,           0x17},
//    {CC112X_FS_PFD,            0x50},
//    {CC112X_FS_PRE,            0x6E},
//    {CC112X_FS_REG_DIV_CML,    0x14},
//    {CC112X_FS_SPARE,          0xAC},
//    {CC112X_XOSC5,             0x0E},
//    {CC112X_XOSC3,             0xC7},
//    {CC112X_XOSC1,             0x07},
//    {CC112X_SERIAL_STATUS,     0x08},
// };

#define UART_INTERFACE UART_DEBUG_4
static uint8_t buffer[17] = "UOS3 UOS3 UOS3 k\0";
static uint32_t buffer_length = 16;
static radio_config_t radio_transmitter = 
{
  .frequency = 145.5,
  .power = 10,
};

// static void cw_tone_on(void)
// {
//   uint8_t pwr_reg;

//   radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));
//   manualCalibration(SPI_RADIO_TX);

//   radio_set_freq_f(SPI_RADIO_TX, &radio_transmitter.frequency);

//   radio_set_pwr_f(SPI_RADIO_TX, &radio_transmitter.power, &pwr_reg);

//   SPI_cmd(SPI_RADIO_TX, CC112X_STX);

//   LED_on(LED_B);
// }

// static void cw_tone_off(void)
// {
//  radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));

//   LED_off(LED_B);
//}

int main(void)
{
  char output[100];

  Board_init();

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nCW Radio Demo\r\n");

  sprintf(output,"Freq: %.3fMHz, Power: %+.1fdBmW\r\n", radio_transmitter.frequency, radio_transmitter.power);
  UART_puts(UART_INTERFACE, output);

  while(1)
  {
    sprintf(output,"Sending Beacon :\"%s\"\r\n", buffer);
    UART_puts(UART_INTERFACE, output);

    Packet_cw_transmit_buffer(buffer, buffer_length, &radio_transmitter, cw_tone_on, cw_tone_off);
    UART_puts(UART_INTERFACE, "Sent.\r\n");

    Delay_ms(3000);
  }
}

/*The old demo cw (not from branch tp)


//#define UART_INTERFACE UART_CAMERA

static double freq = 145.5;
static double pwr = 10.0;
static struct radio_config_t radio_freq_power;
static uint8_t buffer[17] = "UOS3 UOS3 UOS3 k\0";
static uint32_t buffer_length = 16;


static void cw_tone_on(void)
{
//   Enable TX 
  SPI_cmd(SPI_RADIO_TX, CC112X_STX);

  LED_on(LED_B);
}

static void cw_tone_off(void)
{
 //  Enable and calibrate frequency synthesizer 
  SPI_cmd(SPI_RADIO_TX, CC112X_SFSTXON);

  LED_off(LED_B);
}


int main(void)
{
  uint8_t pwr_reg;
  char output[100];
  radio_freq_power.frequency = freq;
  radio_freq_power.power = pwr;

  Board_init();

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nCW Radio Demo\r\n");

  sprintf(output,"Freq: %.3fMHz, Power: %+.1fdBmW\r\n", freq, pwr);
  UART_puts(UART_INTERFACE, output);

  radio_reset_config(SPI_RADIO_TX, &preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));
  manualCalibration(SPI_RADIO_TX);

  radio_set_freq_f(SPI_RADIO_TX, &freq);

  radio_set_pwr_f(SPI_RADIO_TX, &pwr, &pwr_reg);
  
  // Enable and calibrate frequency synthesizer 
  SPI_cmd(SPI_RADIO_TX, CC112X_SFSTXON);

  while(1)
  {
    sprintf(output,"Sending Beacon :\"%s\"\r\n", buffer);
    UART_puts(UART_INTERFACE, output);

    Packet_cw_transmit_buffer(buffer, buffer_length, &radio_freq_power, cw_tone_on, cw_tone_off);
    UART_puts(UART_INTERFACE, "Sent.\r\n");

    Delay_ms(3000);
  }
}
*/