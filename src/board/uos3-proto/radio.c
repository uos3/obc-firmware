/**
 * @ingroup uos3-proto
 * @ingroup radio
 *
 * @file uos3-proto/radio.c
 * @brief Radio Driver - uos3-proto board
 *
 * @{
 */

#include "board.h"
#include "../spi.h"
#include "../gpio.h"
#include "../radio.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

#include "../../cc112x/cc112x.h"

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

#define RADIO_TX_FREQUENCY 145500000 // Hz - TODO: Move this to a central config file

  #if (RADIO_TX_FREQUENCY >= 136700000) && (RADIO_TX_FREQUENCY <= 160000000)
    #define _CC112X_FREQ_DIV  24
    #define _CC112X_BANDSEL   11
  #elif (RADIO_TX_FREQUENCY >= 410000000) && (RADIO_TX_FREQUENCY <= 480000000)
    #define _CC112X_FREQ_DIV  8
    #define _CC112X_BANDSEL   4
  #else
    #error RADIO_TX_FREQUENCY not within defined limits
  #endif
  #define CC112X_FS_CFG_VAL   (0x10 | _CC112X_BANDSEL)

  #define _CC112X_XO_FREQ   38400000 // Hz
  #define _CC112X_FREQ_IVAL (((65536LL * RADIO_TX_FREQUENCY) / _CC112X_XO_FREQ) * _CC112X_FREQ_DIV)

static inline uint8_t Radio_query_partnumber(Radio_device *radio)
{
  return cc112x_query_partnumber(radio->spi_device);
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
  cc112x_reset(Radio_transmitter.spi_device);
  
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
    return RADIO_STATUS_BUSY;
  }

  /* Reset Radio */
  cc112x_reset(Radio_transmitter.spi_device);
  
  if(Radio_query_partnumber(&Radio_transmitter) != Radio_transmitter.device_id)
  {
    return RADIO_STATUS_WRONGPART;
  }

  /* Configure Radio */
  //RADIO_CONFIGURE(Radio_transmitter.spi_device, Radio_Config_TX_FSK);

  return RADIO_STATUS_OK;
}

Radio_Status_t Radio_tx_morse(radio_config_t *radio_config, uint8_t *text_buffer, uint32_t text_length, void *end_of_tx_handler(void))
{
  if(Radio_transmitter.busy)
  {
    return RADIO_STATUS_BUSY;
  }

  /* Reset Radio */
  cc112x_reset(Radio_transmitter.spi_device);
  
  if(Radio_query_partnumber(&Radio_transmitter) != Radio_transmitter.device_id)
  {
    return RADIO_STATUS_WRONGPART;
  }

  /* Configure Radio */

  GPIO_set_risingInterrupt(GPIO0_RADIO_TX, end_of_tx_handler);

  return RADIO_STATUS_OK;
}

Radio_Status_t Radio_tx_off(radio_config_t *radio_config)
{
  (void)radio_config;

  /* Reset Radio */
  cc112x_reset(Radio_transmitter.spi_device);

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