/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

#define RADIO_FREQUENCY 145500000 // Hz
#define RADIO_POWER     1 // dBm
#define RADIO_SYMBOLRATE  10 // Hz - unimplemented

#if (RADIO_FREQUENCY >= 136700000) && (RADIO_FREQUENCY <= 160000000)
  #define _CC112X_FREQ_DIV  24
  #define _CC112X_BANDSEL   11
#elif (RADIO_FREQUENCY >= 410000000) && (RADIO_FREQUENCY <= 480000000)
  #define _CC112X_FREQ_DIV  8
  #define _CC112X_BANDSEL   4
#else
  #error RADIO_FREQUENCY not within defined limits
#endif
#define CC112X_FS_CFG_VAL   (0x10 | _CC112X_BANDSEL)

#define _CC112X_XO_FREQ   38400000 // Hz
#define _CC112X_FREQ_IVAL (((65536LL * RADIO_FREQUENCY) / _CC112X_XO_FREQ) * _CC112X_FREQ_DIV)
#define CC112X_FREQ0_VAL  (_CC112X_FREQ_IVAL & 0xFF)
#define CC112X_FREQ1_VAL  ((_CC112X_FREQ_IVAL >> 8) & 0xFF)
#define CC112X_FREQ2_VAL  ((_CC112X_FREQ_IVAL >> 16) & 0xFF)

#if (RADIO_POWER < -11) || (RADIO_POWER > 15)
  #error RADIO_POWER not within limits (-11 < p < 15) dBm
#endif
#define CC112X_PA_CFG2_VAL  (((2*(RADIO_POWER+18))-1) & 0x3F)

static const registerSetting_t preferredSettings_fsk[]= 
{
  {CC112X_IOCFG3,            0x59},
  {CC112X_IOCFG2,            0x13},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            0x14}, // GPIO0 assert on MCU wakeup
  {CC112X_SYNC_CFG0,         0x00}, // No sync word
  {CC112X_DEVIATION_M,       0xDA},
  {CC112X_MODCFG_DEV_E,      0x18}, // OOK
  {CC112X_PREAMBLE_CFG1,     0x00}, // No preamble
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_FREQ_IF_CFG,       0x40},
  {CC112X_IQIC,              0x46},
  {CC112X_CHAN_BW,           0x69},
  {CC112X_MDMCFG0,           0x05},
  {CC112X_SYMBOL_RATE2,      0x02},
  {CC112X_SYMBOL_RATE1,      0x00},
  {CC112X_SYMBOL_RATE0,      0x00},
  {CC112X_AGC_REF,           0x15},
  {CC112X_AGC_CS_THR,        0x19},
  {CC112X_AGC_CFG1,          0xA0},
  {CC112X_AGC_CFG0,          0xCF},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x1B},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_PKT_CFG1,          0x05},
  {CC112X_PA_CFG0,           0x04 | ((CC112X_PA_CFG2_VAL/4) << 3)}, // 10
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x22},
  {CC112X_IF_ADC0,           0x05},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC3,             0xC7},
  {CC112X_XOSC1,             0x07},
   {CC112X_FREQ0,             CC112X_FREQ0_VAL},
   {CC112X_FREQ1,             CC112X_FREQ1_VAL},
   {CC112X_FREQ2,             CC112X_FREQ2_VAL},
   {CC112X_FS_CFG,            CC112X_FS_CFG_VAL},
   {CC112X_PA_CFG2,           CC112X_PA_CFG2_VAL},
};

static const registerSetting_t preferredSettings_cw[]= 
{
   {CC112X_IOCFG3,            0x59},
   {CC112X_IOCFG2,            0x13},
   {CC112X_IOCFG1,            0xB0},
   {CC112X_IOCFG0,            0x06},
   {CC112X_SYNC_CFG0,         0x00}, // No sync word
   {CC112X_SYNC_CFG1,         0x08},
   {CC112X_DEVIATION_M,       0xB4},
   {CC112X_MODCFG_DEV_E,      0x1A}, // OOK, Freq dev exponent = 0x2
   {CC112X_DCFILT_CFG,        0x1C},
   {CC112X_PREAMBLE_CFG1,     0x00}, // No preamble
   {CC112X_FREQ_IF_CFG,       0x35},
   {CC112X_IQIC,              0xC6},
   {CC112X_CHAN_BW,           0x10},
   {CC112X_MDMCFG1,           0x46}, // Enable FIFO Mode
   {CC112X_MDMCFG0,           0x05},
   {CC112X_SYMBOL_RATE2,      0x40},
   {CC112X_SYMBOL_RATE1,      0x62},
   {CC112X_SYMBOL_RATE0,      0x4E},
   {CC112X_AGC_REF,           0x20},
   {CC112X_AGC_CS_THR,        0x19},
   {CC112X_AGC_CFG1,          0xA9},
   {CC112X_AGC_CFG0,          0xCF},
   {CC112X_FIFO_CFG,          0x00},
   {CC112X_SETTLING_CFG,      0x03},
   {CC112X_PKT_CFG2,          0x04}, // Normal (FIFO) Packet Mode
   {CC112X_PKT_CFG1,          0x00}, // No whitening, no CRC
   {CC112X_PKT_CFG0,          0x20}, // Variable packet length mode (first byte in fifo is length)
   {CC112X_PA_CFG0,           0x7E},
   {CC112X_IF_MIX_CFG,        0x00},
   {CC112X_FREQOFF_CFG,       0x22},
   {CC112X_CFM_DATA_CFG,      0x01},
   {CC112X_IF_ADC0,           0x05},
   {CC112X_FS_DIG1,           0x00},
   {CC112X_FS_DIG0,           0x5F},
   {CC112X_FS_CAL0,           0x0E},
   {CC112X_FS_DIVTWO,         0x03},
   {CC112X_FS_DSM0,           0x33},
   {CC112X_FS_DVC0,           0x17},
   {CC112X_FS_PFD,            0x50},
   {CC112X_FS_PRE,            0x6E},
   {CC112X_FS_REG_DIV_CML,    0x14},
   {CC112X_FS_SPARE,          0xAC},
   {CC112X_XOSC5,             0x0E},
   {CC112X_XOSC3,             0xC7},
   {CC112X_XOSC1,             0x07},
   //{CC112X_SERIAL_STATUS,     0x08},
   {CC112X_FREQ0,             CC112X_FREQ0_VAL},
   {CC112X_FREQ1,             CC112X_FREQ1_VAL},
   {CC112X_FREQ2,             CC112X_FREQ2_VAL},
   {CC112X_FS_CFG,            CC112X_FS_CFG_VAL},
   {CC112X_PA_CFG2,           CC112X_PA_CFG2_VAL},
};

#define UART_INTERFACE UART_CAMERA

static uint8_t buffer[17] = "UOS3 UOS3 UOS3 k\0";
static uint32_t buffer_length = 16;

static void cw_tone_on(void)
{
  /* Enable TX */
  SPI_cmd(SPI_RADIO_TX, CC112X_STX);

  LED_on(LED_B);
}

static void cw_tone_off(void)
{
  /* IDLE to switch off TX and frequency synthesizer */
  SPI_cmd(SPI_RADIO_TX, CC112X_SIDLE);
  /* Enable frequency synthesizer to hold keep oscillator warm */
  SPI_cmd(SPI_RADIO_TX, CC112X_SFSTXON);

  LED_off(LED_B);
}

static volatile bool tx_end_trigger;

static void set_tx_end_trigger(void)
{
  tx_end_trigger = true;
}

int main(void)
{
  uint8_t marcstate;
  char output[100];

  Board_init();

  UART_init(UART_INTERFACE, 9600);
  UART_puts(UART_INTERFACE, "\r\nCW Radio Demo\r\n");

  sprintf(output,"Freq: %d Hz, Power: %+d dBmW\r\n", RADIO_FREQUENCY, RADIO_POWER);
  UART_puts(UART_INTERFACE, output);

  /* Wait for radio to initialise (CHIP_RDYn) */
  while(SPI_cmd(SPI_RADIO_TX, CC112X_SNOP) & 0x80)
  {
    Delay_ms(1);
  }

  //radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));
  radio_reset_config(SPI_RADIO_TX, preferredSettings_fsk, sizeof(preferredSettings_fsk)/sizeof(registerSetting_t));
  manualCalibration(SPI_RADIO_TX);
  SPI_cmd(SPI_RADIO_TX, CC112X_SIDLE);

  cc112xSpiReadReg(SPI_RADIO_TX, CC112X_MARC_STATUS1, &marcstate);

  /* Set up TX_GPIO0 Interrupt */
  tx_end_trigger = false;
  GPIO_set_risingInterrupt(GPIO0_RADIO_TX, set_tx_end_trigger);

  while(1)
  {
    uint8_t buff[256];
    uint8_t i;
    buff[0] = 12;
    for (i = 1; i < (buff[0]+1); i++)
    {
      buff[i] = (uint8_t)(127-i);
    }
    
    cc112xSpiWriteTxFifo(SPI_RADIO_TX, buff, (buff[0]+1));

    tx_end_trigger = false;
    SPI_cmd(SPI_RADIO_TX, CC112X_STX);

    LED_on(LED_B);
    UART_puts(UART_INTERFACE, "Transmitting\r\n");
    // wait for the packet to send

    do
    {
      while( !tx_end_trigger) { };
      tx_end_trigger = false;
      cc112xSpiReadReg(SPI_RADIO_TX, CC112X_MARC_STATUS1, &marcstate);
    }
    while (marcstate != 0x40); // Wait for TX finished ok

    LED_off(LED_B);
    UART_puts(UART_INTERFACE, "Done\r\n");

    Delay_ms(1000);
  }
}
