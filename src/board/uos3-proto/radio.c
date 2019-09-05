/**
 * @ingroup uos3-proto
 * @ingroup radio
 *
 * @file uos3-proto/radio.c
 * @brief Radio Driver - uos3-proto board
 *
 * @{
 */
#include "../../firmware.h"
// #include "board.h"
// #include "../spi.h"
// #include "../gpio.h"
// #include "../radio.h"
// #include "../../cc112x/cc112x.h"
//#include "../../cc1125/cc1125.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"


const registerSetting_t preferredSettings_fsk[]= 
{
  {CC112X_IOCFG3,            0x59},
  {CC112X_IOCFG2,            0x13},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            0x06},
  {CC112X_SYNC_CFG1,         0x0B},
  {CC112X_DEVIATION_M,       0xDA},   //1KHz DEVIATION
  {CC112X_MODCFG_DEV_E,      0x00},   //2-fsk  0 deviation  exponent
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_FREQ_IF_CFG,       0x40},
  {CC112X_IQIC,              0x46},
  {CC112X_CHAN_BW,           0x69},
  {CC112X_MDMCFG0,           0x05},
  {CC112X_SYMBOL_RATE2,      0x3B}, //1000bps symbol rate
  {CC112X_SYMBOL_RATE1,      0x4E},
  {CC112X_SYMBOL_RATE0,      0x82},
  {CC112X_AGC_REF,           0x15},
  {CC112X_AGC_CS_THR,        0x19},
  {CC112X_AGC_CFG1,          0xA0},
  {CC112X_AGC_CFG0,          0xCF},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x13},//0x03}, //Added autocalibration
  {CC112X_FS_CFG,            0x1B},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_PKT_CFG1,          0x05},
  {CC112X_PA_CFG0,           0x7E},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x22},
  {CC112X_FREQ2,             0x5A},
  {CC112X_FREQ1,             0xEF},//0x9F},
  {CC112X_FREQ0,             0xFF},
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
};
//pkt cfg2 set to default


// const registerSetting_t preferredSettings_cw[]= 
// {
//    {CC112X_IOCFG3,            0x59},
//    {CC112X_IOCFG2,            0x13},
//    {CC112X_IOCFG1,            0xB0},
//    {CC112X_IOCFG0,            0x06},
//    {CC112X_SYNC_CFG1,         0x08},  //Sync threshold value of 4
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
//    {CC112X_SETTLING_CFG,      0x13},//0x03},//Added autocalibration
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

//From demo phil cw
// static const registerSetting_t preferredSettings_cw[]= 
// {
//    {CC112X_IOCFG3,            0x59},  //used to control external PA active low
//    {CC112X_IOCFG2,            0x13},  //Always on in tx , crc in rx
//    {CC112X_IOCFG1,            0xB0},  //High impedance
//    {CC112X_IOCFG0,            0x06},  //Goes high when sync word sent, goes low when packet ends/overflow error
//    {CC112X_SYNC_CFG0,         0x00}, // No sync word
//    {CC112X_SYNC_CFG1,         0x08},
//    {CC112X_DEVIATION_M,       0xB4},
//    {CC112X_MODCFG_DEV_E,      0x1A}, // OOK, Freq dev exponent = 0x2
//    {CC112X_DCFILT_CFG,        0x1C},
//    {CC112X_PREAMBLE_CFG1,     0x00}, // No preamble
//    {CC112X_FREQ_IF_CFG,       0x35},
//    {CC112X_IQIC,              0xC6},
//    {CC112X_CHAN_BW,           0x10},
//    {CC112X_MDMCFG1,           0x46}, // Enable FIFO Mode
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
//    {CC112X_PKT_CFG2,          0x04}, // Normal (FIFO) Packet Mode
//    {CC112X_PKT_CFG1,          0x00}, // No whitening, no CRC
//    {CC112X_PKT_CFG0,          0x20}, // Variable packet length mode (first byte in fifo is length)
//    {CC112X_PA_CFG0,           0x7E},
//    {CC112X_IF_MIX_CFG,        0x00},
//    {CC112X_FREQOFF_CFG,       0x22},
//    {CC112X_CFM_DATA_CFG,      0x01},
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
//    //{CC112X_SERIAL_STATUS,     0x08},
// };
  /*
  {CC112X_SYNC_CFG0,         0x00}, // No sync word
  {CC112X_DEVIATION_M,       0xDA},
  {CC112X_MODCFG_DEV_E,      0x18}, // OOK
  {CC112X_PREAMBLE_CFG1,     0x00}, // No preamble
  */
static const registerSetting_t preferredSettings_cw[]= 
{
   {CC112X_IOCFG3,         0x59},
   {CC112X_IOCFG2,         0x13},
   {CC112X_IOCFG1,         0xB0},
   {CC112X_IOCFG0,            0x06},
   {CC112X_SYNC_CFG1,         0x08},
   {CC112X_DEVIATION_M,       0xB4},
   {CC112X_MODCFG_DEV_E,      0x0A},  //GFSK, Deviation exponent 010
   {CC112X_DCFILT_CFG,        0x1C},
   {CC112X_PREAMBLE_CFG1,     0x00},
   {CC112X_FREQ_IF_CFG,       0x35},
   {CC112X_IQIC,              0xC6},
   {CC112X_CHAN_BW,           0x10},
   {CC112X_MDMCFG1,           0x06},
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
   {CC112X_FS_CFG,            0x1B},
   {CC112X_PKT_CFG2,          0x05},
   {CC112X_PKT_CFG1,          0x00},
   {CC112X_PKT_CFG0,          0x20},
   {CC112X_PA_CFG2,           0x34},
   {CC112X_PA_CFG0,           0x7E},
   {CC112X_IF_MIX_CFG,        0x00},
   {CC112X_FREQOFF_CFG,       0x22},
   {CC112X_CFM_DATA_CFG,      0x01},
   {CC112X_FREQ2,             0x5A},
   {CC112X_FREQ1,             0xEF},
   {CC112X_FREQ0,             0xFF},
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
   {CC112X_SERIAL_STATUS,     0x08},
};
typedef struct Radio_device {
  uint8_t   spi_device;
  uint8_t   device_id;
  bool    busy;
} Radio_device;

static Radio_device Radio_transmitter =
{
  .spi_device = SPI_RADIO_TX,
  .device_id = 0x8F, // cc1125
  .busy = false,
};
static Radio_device Radio_receiver =
{
  .spi_device = SPI_RADIO_RX,
  .device_id = 0x8F, // cc1125
  .busy = false,
};


static inline uint8_t Radio_query_partnumber(Radio_device *radio)
{
  return cc112x_query_partnumber(Radio_transmitter.spi_device);
}

/* This is a macro to preserve config size information */
#define RADIO_CONFIGURE(_radio_id,_cfg)   uint8_t writebyte; \
                                          for(uint16_t i = 0; i < (sizeof(_cfg) / sizeof(radio_config_register_t)); i++) { \
                                            writeByte = _cfg[i].data; \
                                            cc112xSpiWriteReg(_radio_id, _cfg[i].addr, &writeByte); \
                                          }


/** Public Functions */


Radio_Status_t Radio_tx_msk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler(void))
{
  if(Radio_transmitter.busy)
  {
    return RADIO_STATUS_BUSY;
  }

  /* Reset Radio */
  SPI_cmd(Radio_transmitter.spi_device, CC112X_SRES);
  
  if(Radio_query_partnumber(&Radio_transmitter) != Radio_transmitter.device_id)
  {
    return RADIO_STATUS_WRONGPART;
  }

  /* Configure Radio */


  return RADIO_STATUS_OK;
}

Radio_Status_t Radio_tx_fsk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler(void))
{

  if(Radio_transmitter.busy)
  {
    UART_puts(UART_INTERFACE, "Radio busy\r\n");

    return RADIO_STATUS_BUSY;
  }

  /* Reset Radio */
  SPI_cmd(Radio_transmitter.spi_device, CC112X_SRES);
  

  if (!(cc112x_query_partnumber(Radio_transmitter.spi_device))) {
    UART_puts(UART_INTERFACE, "Part num query failed\r\n");
    return;
  }
  // if(Radio_query_partnumber(&Radio_transmitter) != Radio_transmitter.device_id)
  // {
  //   UART_puts(UART_INTERFACE, "Part num query failed\r\n");
  //   return RADIO_STATUS_WRONGPART;
  // }

  /* Configure Radio */
  cc112x_set_config(Radio_transmitter.spi_device, preferredSettings_fsk, sizeof(preferredSettings_fsk)/sizeof(registerSetting_t));
  //Better to be hard coded in the config above
  //cc112x_cfg_frequency(Radio_transmitter.spi_device, (float) radio_config->frequency); //set frequency according to the config file passed as argument
  cc112x_cfg_power(Radio_transmitter.spi_device, radio_config->power);  //set power according to the config file passed as argument
  //cc112x_cfg_fsk_params(Radio_transmitter.spi_device, CC112X_FSK_SYMBOLRATE_50, CC112X_FSK_DEVIATION_8K);
  cc112x_manualCalibration(Radio_transmitter.spi_device); //calibrate the radio
  UART_puts(UART_INTERFACE, "Calibrated\r\n");

  //Load tx fifo
  cc112xSpiWriteTxFifo(Radio_transmitter.spi_device, data_buffer, data_length);
  UART_puts(UART_INTERFACE, "Loaded to FIFO\r\n");

  //Printing
  char output[100];
  uint8_t val = 0;
  cc112xSpiReadReg(Radio_transmitter.spi_device, CC112X_MODCFG_DEV_E, &val);
  sprintf(output,"Modulation reg %x\r\n", val);
  UART_puts(UART_INTERFACE, output);

  SPI_cmd(Radio_transmitter.spi_device, CC112X_STX);  //enable TX operation
  UART_puts(UART_INTERFACE, "Transmitting\r\n");

  return RADIO_STATUS_OK;
}


//Currently setting freq, power and manual calibration don't work
void cw_tone_on(radio_config_t *radio_config)   {
  //Checking the transmitter is switched on otherwise it wont be detected until halfway through manual calibration resulting in
  //an out of band transmission
  if (!(cc112x_query_partnumber(Radio_transmitter.spi_device))) {
    UART_puts(UART_INTERFACE, "Part num query failed\r\n");
    return;
  }
  cc112x_set_config(Radio_transmitter.spi_device, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));

  //Better to be hardcoded
  //cc112x_cfg_frequency(Radio_transmitter.spi_device, (float) radio_config->frequency); //set frequency according to the config file passed as argument
  cc112x_cfg_power(Radio_transmitter.spi_device, radio_config->power);  //set power according to the config file passed as argument
  cc112x_manualCalibration(Radio_transmitter.spi_device); //calibrate the radio

  UART_puts(UART_INTERFACE, "Beginning Transmit\r\n");
  SPI_cmd(Radio_transmitter.spi_device, CC112X_STX);  //enable TX operation
  LED_on(LED_B);
}

void cw_tone_off(void)  {
  SPI_cmd(Radio_transmitter.spi_device, CC112X_SIDLE); //exit RX/TX, turn off freq. synthesizer; no reseting and writing basic config. because it is done in cw_tone_on function
  LED_off(LED_B);
}

Radio_Status_t Radio_tx_morse(radio_config_t *radio_config, uint8_t *text_buffer, uint32_t text_length, void *end_of_tx_handler(void))
{
  if(Radio_transmitter.busy)
  {
    return RADIO_STATUS_BUSY;
  }

  /* Reset Radio */
  SPI_cmd(Radio_transmitter.spi_device, CC112X_SRES);
  
  if(Radio_query_partnumber(&Radio_transmitter) != Radio_transmitter.device_id)
  {
    return RADIO_STATUS_WRONGPART;
  }

  /* Configure Radio */
  Packet_cw_transmit_buffer(text_buffer, text_length, radio_config, cw_tone_on, cw_tone_off);

  GPIO_set_risingInterrupt(GPIO0_RADIO_TX, end_of_tx_handler);

  return RADIO_STATUS_OK;
}

Radio_Status_t Radio_tx_off(radio_config_t *radio_config)
{
  (void)radio_config;

  /* Reset Radio */
  SPI_cmd(Radio_transmitter.spi_device, CC112X_SRES);

  Radio_transmitter.busy = false;
  
  /* Test we can still talk to it */
  if(Radio_query_partnumber(&Radio_transmitter) != Radio_transmitter.device_id)
  {
    return RADIO_STATUS_WRONGPART;
  }

  /* Power Down Radio */
  cc112x_powerdown(Radio_transmitter.spi_device);

  return RADIO_STATUS_OK;
}


Radio_Status_t Radio_rx_receive(radio_config_t *radio_config, uint8_t *receive_buffer, uint32_t receive_length, void received_packet_handler(void))
{
  (void)radio_config;
  (void)receive_buffer;
  (void)receive_length;
  (void)received_packet_handler;


  return RADIO_STATUS_OK;
}

Radio_Status_t Radio_rx_off(radio_config_t *radio_config)
{
  (void)radio_config;

  return RADIO_STATUS_OK;
}

/**
 * @}
 */