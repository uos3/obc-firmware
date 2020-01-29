#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

#include "../driver/board.h"
#include "radio.h"
#define minRxPktLen 127    //(bytes) needs updating when packet structures fixed, not including preamble, sync words etc
//Not needed if we sort variable packet length and change planned length by including it after sync words


// RX filter BW = 50.000000
// Address config = No address check
// Packet length = 125
// Symbol rate = 1.2
// PA ramping = true
// Performance mode = High Performance
// Carrier frequency = 145.500000
// Crystal: 38.400
// Bit rate = 1.2
// Packet bit length = 0
// Whitening = false
// Manchester enable = false
// Modulation format = 2-FSK
// Packet length mode = Variable
// Device address = 0
// TX power = 15
// Deviation = 20.019531
// Rf settings for CC1120


//Wake on receive
static const registerSetting_t preferredSettings_rxfsk[] = {
    {CC112X_IOCFG3,         0x59},
    {CC112X_IOCFG2,         0x13},
    {CC112X_IOCFG1,         0xB0},
    {CC112X_IOCFG0,         0x01}, //Was 6, now 1 when data in rx buffer exceeds minRxPktLen or packet end reached
    {CC112X_SYNC_CFG1,      0x0B},
    {CC112X_DEVIATION_M,    0x48},
    {CC112X_MODCFG_DEV_E,   0x05},
    {CC112X_DCFILT_CFG,     0x1C},
    {CC112X_IQIC,           0x00},
    {CC112X_CHAN_BW,        0x04},
    {CC112X_MDMCFG0,        0x05},
    {CC112X_AGC_CS_THR,     0xF5},
    {CC112X_AGC_CFG1,       0xA0},
    {CC112X_SETTLING_CFG,   0x03},
    {CC112X_FS_CFG,         0x1B},
    {CC112X_WOR_CFG0,       0x20},
    {CC112X_WOR_EVENT0_MSB, 0x02},
    {CC112X_WOR_EVENT0_LSB, 0x14},
    {CC112X_PKT_CFG0,       0x20},
	  {CC112X_PKT_CFG1,       0x05},
    {CC112X_RFEND_CFG0,     0x09},
    {CC112X_PKT_LEN,        0x7D},
    {CC112X_IF_MIX_CFG,     0x00},
    {CC112X_FREQOFF_CFG,    0x22},
    {CC112X_FREQ2,          0x5A},
    {CC112X_FREQ1,          0xF0},
    {CC112X_FREQ0,          0x00},
    {CC112X_FS_DIG1,        0x00},
    {CC112X_FS_DIG0,        0x5F},
    {CC112X_FS_CAL1,        0x40},
    {CC112X_FS_CAL0,        0x0E},
    {CC112X_FS_DIVTWO,      0x03},
    {CC112X_FS_DSM0,        0x33},
    {CC112X_FS_DVC0,        0x17},
    {CC112X_FS_PFD,         0x50},
    {CC112X_FS_PRE,         0x6E},
    {CC112X_FS_REG_DIV_CML, 0x14},
    {CC112X_FS_SPARE,       0xAC},
    {CC112X_FS_VCO0,        0xB4},
    {CC112X_XOSC5,          0x0E},
    {CC112X_XOSC1,          0x03},
    {CC112X_FIFO_CFG,       (uint8_t) minRxPktLen},

};


//Must set deviation to 1/4 of symbol rate
static const registerSetting_t preferredSettings_rxmsk[] = {
    {CC112X_IOCFG3,         0x59},
    {CC112X_IOCFG2,         0x13},
    {CC112X_IOCFG1,         0xB0},
    {CC112X_IOCFG0,         0x06},
    {CC112X_SYNC_CFG1,      0x0B},
    {CC112X_DEVIATION_M,    0x48},
    {CC112X_MODCFG_DEV_E,   0x0D},//Set to 2 gfsk to receive msk
    {CC112X_DCFILT_CFG,     0x1C},
    {CC112X_IQIC,           0x00},
    {CC112X_CHAN_BW,        0x04},
    {CC112X_MDMCFG0,        0x05},
    {CC112X_AGC_CS_THR,     0xF5},
    {CC112X_AGC_CFG1,       0xA0},
    {CC112X_SETTLING_CFG,   0x03},
    {CC112X_FS_CFG,         0x1B},
    {CC112X_WOR_CFG0,       0x20},
    {CC112X_WOR_EVENT0_MSB, 0x02},
    {CC112X_WOR_EVENT0_LSB, 0x14},
    {CC112X_PKT_CFG0,       0x20},
	  {CC112X_PKT_CFG1,       0x05},
    {CC112X_RFEND_CFG0,     0x09},
    {CC112X_PKT_LEN,        0x7D},
    {CC112X_IF_MIX_CFG,     0x00},
    {CC112X_FREQOFF_CFG,    0x22},
    {CC112X_FREQ2,          0x5A},
    {CC112X_FREQ1,          0xF0},
    {CC112X_FREQ0,          0x00},
    {CC112X_FS_DIG1,        0x00},
    {CC112X_FS_DIG0,        0x5F},
    {CC112X_FS_CAL1,        0x40},
    {CC112X_FS_CAL0,        0x0E},
    {CC112X_FS_DIVTWO,      0x03},
    {CC112X_FS_DSM0,        0x33},
    {CC112X_FS_DVC0,        0x17},
    {CC112X_FS_PFD,         0x50},
    {CC112X_FS_PRE,         0x6E},
    {CC112X_FS_REG_DIV_CML, 0x14},
    {CC112X_FS_SPARE,       0xAC},
    {CC112X_FS_VCO0,        0xB4},
    {CC112X_XOSC5,          0x0E},
    {CC112X_XOSC1,          0x03},
};

const registerSetting_t preferredSettings_fsk[]= 
{
  {CC112X_IOCFG3,            0x59},
  {CC112X_IOCFG2,            0x13},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            0x06},
  {CC112X_SYNC_CFG1,         0x0B},
  // {CC112X_DEVIATION_M,       0xDA},   //1KHz DEVIATION
  // {CC112X_MODCFG_DEV_E,      0x00},   //2-fsk  0 deviation  exponent
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_FREQ_IF_CFG,       0x40},
  {CC112X_IQIC,              0x46},
  {CC112X_CHAN_BW,           0x69},
  {CC112X_MDMCFG0,           0x05},
  // {CC112X_SYMBOL_RATE2,      0x3B}, //1000bps symbol rate
  // {CC112X_SYMBOL_RATE1,      0x4E},
  // {CC112X_SYMBOL_RATE0,      0x82},
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
  //Settings are identical apart from cfg_msk_params
  cc112x_set_config(Radio_transmitter.spi_device, preferredSettings_fsk, sizeof(preferredSettings_fsk)/sizeof(registerSetting_t));
  //Better to be hard coded in the config above
  //cc112x_cfg_frequency(Radio_transmitter.spi_device, (float) radio_config->frequency); //set frequency according to the config file passed as argument
  cc112x_cfg_power(Radio_transmitter.spi_device, radio_config->power);  //set power according to the config file passed as argument
  cc112x_cfg_fsk_params(Radio_transmitter.spi_device, radio_config->symbolRate, CC112X_FSK_DEVIATION_500);
  cc112x_manualCalibration(Radio_transmitter.spi_device); //calibrate the radio
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, "Calibrated\r\n");
  #endif
  //Load tx fifo
  cc112xSpiWriteTxFifo(Radio_transmitter.spi_device, data_buffer, data_length);
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, "Loaded to FIFO\r\n");
  #endif
  SPI_cmd(Radio_transmitter.spi_device, CC112X_STX);  //enable TX operation
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, "Transmitting\r\n");
  #endif
  return RADIO_STATUS_OK;
}

Radio_Status_t Radio_tx_fsk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler(void))
{

  if(Radio_transmitter.busy)
  {
    #ifdef DEBUG_MODE
      UART_puts(UART_INTERFACE, "Radio busy\r\n");
    #endif
    return RADIO_STATUS_BUSY;
  }

  /* Reset Radio */
  SPI_cmd(Radio_transmitter.spi_device, CC112X_SRES);
  

  if (!(cc112x_query_partnumber(Radio_transmitter.spi_device))) {
    #ifdef DEBUG_MODE
      UART_puts(UART_INTERFACE, "Part num query failed\r\n");
    #endif
    return;
  }

  /* Configure Radio */
  cc112x_set_config(Radio_transmitter.spi_device, preferredSettings_fsk, sizeof(preferredSettings_fsk)/sizeof(registerSetting_t));
  //Better to be hard coded in the config above
  //cc112x_cfg_frequency(Radio_transmitter.spi_device, (float) radio_config->frequency); //set frequency according to the config file passed as argument
  cc112x_cfg_power(Radio_transmitter.spi_device, radio_config->power);  //set power according to the config file passed as argument
  cc112x_cfg_fsk_params(Radio_transmitter.spi_device, radio_config->symbolRate, CC112X_FSK_DEVIATION_8K);
  cc112x_manualCalibration(Radio_transmitter.spi_device); //calibrate the radio
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, "Calibrated\r\n");
  #endif
  //Load tx fifo
  cc112xSpiWriteTxFifo(Radio_transmitter.spi_device, data_buffer, data_length);
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, "Loaded to FIFO\r\n");
  #endif
  SPI_cmd(Radio_transmitter.spi_device, CC112X_STX);  //enable TX operation
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, "Transmitting\r\n");
  #endif
  return RADIO_STATUS_OK;
}


//Currently setting freq, power and manual calibration don't work
void cw_tone_on(radio_config_t *radio_config)   {
  //Checking the transmitter is switched on otherwise it wont be detected until halfway through manual calibration resulting in
  //an out of band transmission
  if (!(cc112x_query_partnumber(Radio_transmitter.spi_device))) {
    #ifdef DEBUG_MODE
      UART_puts(UART_INTERFACE, "Part num query failed\r\n");
    #endif
    return;
  }
  cc112x_set_config(Radio_transmitter.spi_device, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));

  //Better to be hardcoded
  //cc112x_cfg_frequency(Radio_transmitter.spi_device, (float) radio_config->frequency); //set frequency according to the config file passed as argument
  cc112x_cfg_power(Radio_transmitter.spi_device, radio_config->power);  //set power according to the config file passed as argument
  cc112x_manualCalibration(Radio_transmitter.spi_device); //calibrate the radio
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, "Beginning Transmit\r\n");
  #endif
  SPI_cmd(Radio_transmitter.spi_device, CC112X_STX);  //enable TX operation
  LED_on(LED_B);
}

void cw_tone_off(void)  {
  SPI_cmd(Radio_transmitter.spi_device, CC112X_SIDLE); //exit RX/TX, turn off freq. synthesizer; no reseting and writing basic config. because it is done in cw_tone_on function
  LED_off(LED_B);
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



//Enables wake on receive, sets config for receiving fsk data and puts the radio into receive mode
//TODO sort received packet handler
Radio_Status_t Radio_enable_rx_ewor_fsk(radio_config_t *radio_config, uint8_t *rxBuffer)
{
    if(Radio_transmitter.busy)
  {
    #ifdef DEBUG_MODE
      UART_puts(UART_INTERFACE, "Radio busy\r\n");
    #endif
    return RADIO_STATUS_BUSY;
  }

  /* Reset Radio */
  SPI_cmd(Radio_receiver.spi_device, CC112X_SRES);
  
  //Check it will actually receive settings
  if (!(cc112x_query_partnumber(Radio_receiver.spi_device))) {
    #ifdef DEBUG_MODE
      UART_puts(UART_INTERFACE, "Part num query failed\r\n");
    #endif
    return;
  }

    /* Configure Radio */
  cc112x_set_config(Radio_receiver.spi_device, preferredSettings_rxfsk, sizeof(preferredSettings_rxfsk)/sizeof(registerSetting_t));
  //Better to be hard coded in the config above
  //cc112x_cfg_frequency(Radio_transmitter.spi_device, (float) radio_config->frequency); //set frequency according to the config file passed as argument
  cc112x_cfg_fsk_params(Radio_receiver.spi_device, radio_config->symbolRate, CC112X_FSK_DEVIATION_500);
  cc112x_manualCalibration(Radio_receiver.spi_device); //calibrate the radio
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, "Calibrated\r\n");
  #endif
  //SPI_cmd(Radio_receiver.spi_device, CC112X_SRX);  //enable RX operation
  #ifdef DEBUG_MODE
    UART_puts(UART_INTERFACE, "Receive mode\r\n");
  #endif
  return RADIO_STATUS_OK;
}



Radio_Status_t Radio_read_rx_fifo(uint8_t *rxBuffer) {

  uint8_t marcState, rxBytes;

  //Read MARCSTATE to check for RX FIFO error
  cc112xSpiReadReg(Radio_receiver.spi_device, CC112X_MARCSTATE, &marcState);

  // Mask out MARCSTATE bits and check if we have a RX FIFO error
  if((marcState & 0x1F) == 0x11) {
    #ifdef DEBUG_MODE
      UART_puts(UART_INTERFACE, "RX FIFO error :(\n");
    #endif
    // Flush RX FIFO
    SPI_cmd(Radio_receiver.spi_device, CC112X_SFRX);
    
  } else {
    // Read n bytes from RX FIFO
    cc112xSpiReadReg(Radio_receiver.spi_device, CC112X_NUM_RXBYTES, &rxBytes);
    cc112xSpiReadRxFifo(Radio_receiver.spi_device, rxBuffer, rxBytes);
    
  }
}


Radio_Status_t Radio_rx_off(radio_config_t *radio_config)
{
  (void)radio_config;

  return RADIO_STATUS_OK;
}


/**
 * @}
 */