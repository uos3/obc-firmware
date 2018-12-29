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

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

typedef struct Radio_device {
  uint8_t   spi_device;
  uint8_t   device_id;
} Radio_device;

static Radio_device Radio_transmitter =
{
  .spi_device = SPI_RADIO_TX,
  .device_id = 0x8F, // cc1125
};
static Radio_device Radio_receiver =
{
  .spi_device = SPI_RADIO_RX,
  .device_id = 0x8F, // cc1125
};

static uint8_t Radio_query_partnumber(Radio_device *radio)
{
  uint8_t result;
  cc112xSpiReadReg(radio->spi_device, CC112X_PARTNUMBER, &result);
  return result;
}

static void Radio_configure(Radio_device *radio, radio_config_t *config)
{
  
}

/** Public Functions */


uint8_t Radio_tx_msk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler)
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

void Radio_tx_fsk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler)
{
  Radio_init(&Radio_transmitter);

}

void Radio_tx_morse(radio_config_t *radio_config, uint8_t *text_buffer, uint32_t text_length, void *end_of_tx_handler)
{
  Radio_init(&Radio_transmitter);

}

void Radio_tx_off(radio_config_t *radio_config)
{
  Radio_init(&Radio_transmitter);

}


void Radio_rx_receive(radio_config_t *radio_config, void *received_packet_handler);

void Radio_rx_off(radio_config_t *radio_config);







uint8_t SPI_cmd(uint8_t spi_num, uint8_t cmd)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Send Command */
  SSIDataPut(spi->port->base_spi, (uint32_t)cmd);
  SSIDataGet(spi->port->base_spi, &r);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_read8(uint8_t spi_num, uint8_t addr, uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Write address, read status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)(addr));
  SSIDataGet(spi->port->base_spi, &r);

  /* Read Data */
  SSIDataPut(spi->port->base_spi, 0);
  SSIDataGet(spi->port->base_spi, &d);
  *data = (uint8_t)d;

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstread8(uint8_t spi_num, uint8_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit address with Read Bit Set */
  SSIDataPut(spi->port->base_spi, (uint32_t)(addr));
  SSIDataGet(spi->port->base_spi, &r);

  while(len--)
  {
    SSIDataPut(spi->port->base_spi, 0);
    SSIDataGet(spi->port->base_spi, &d);
    *data++ = (uint8_t)d;
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_write8(uint8_t spi_num, uint8_t addr, uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Write Address, read status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr);
  SSIDataGet(spi->port->base_spi, &r);

  if(data!=NULL)
  {
    /* Write Data */
    w = *data;
    SSIDataPut(spi->port->base_spi, w);
    SSIDataGet(spi->port->base_spi, &d);
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstwrite8(uint8_t spi_num, uint8_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Write Address, read status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr);
  SSIDataGet(spi->port->base_spi, &r);

  /* Write data */
  while(len--)
  {
    w = *data;
    SSIDataPut(spi->port->base_spi, w);
    SSIDataGet(spi->port->base_spi, &d);
    data++;
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_read16(uint8_t spi_num, uint16_t addr, uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  uint8_t addr_msb  = (uint8_t)(addr >> 8);
  uint8_t addr_lsb = (uint8_t)(addr & 0x00FF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }
 
  /* Transmit MSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Transmit LSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataGet(spi->port->base_spi, &d);

  /* Read Result */
  SSIDataPut(spi->port->base_spi, 0);
  SSIDataGet(spi->port->base_spi, &d);
  *data = (uint8_t)d;

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstread16(uint8_t spi_num, uint16_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  uint8_t addr_msb  = (uint8_t)(addr >> 8);
  uint8_t addr_lsb = (uint8_t)(addr & 0x00FF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Transmit LSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataGet(spi->port->base_spi, &d);

  while(len--)
  {
    SSIDataPut(spi->port->base_spi, 0);
    SSIDataGet(spi->port->base_spi, &d);
    *data++ = (uint8_t)d;
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_write16(uint8_t spi_num, uint16_t addr, uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  uint8_t addr_msb  = (uint8_t)(addr >> 8);
  uint8_t addr_lsb = (uint8_t)(addr & 0x00FF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)(0x7F & addr_msb));
  SSIDataGet(spi->port->base_spi, &r);

  /* Transmit LSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataGet(spi->port->base_spi, &d);

  w = *data;
  SSIDataPut(spi->port->base_spi, w);
  SSIDataGet(spi->port->base_spi, &d);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstwrite16(uint8_t spi_num, uint16_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  uint8_t addr_msb  = (uint8_t)(addr >> 8);
  uint8_t addr_lsb = (uint8_t)(addr & 0x00FF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Transmit LSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataGet(spi->port->base_spi, &d);

  while(len--)
  {
    w = *(data++);
    SSIDataPut(spi->port->base_spi, w);
    SSIDataGet(spi->port->base_spi, &d);
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_read32(uint8_t spi_num, uint32_t addr, uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  uint8_t addr_mmsb  = (uint8_t)(addr >> 24);
  uint8_t addr_msb  = (uint8_t)(addr >> 16);
  uint8_t addr_lsb  = (uint8_t)(addr >> 8);
  uint8_t addr_llsb = (uint8_t)(addr & 0xFF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }
 
  /* Transmit MSB address, reading back status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_mmsb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Send less significant address bytes */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_llsb);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* Flush RX FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* Read Data */
  SSIDataPut(spi->port->base_spi, 0);
  SSIDataGet(spi->port->base_spi, &d);
  *data = (uint8_t)d;

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstread32(uint8_t spi_num, uint32_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  uint8_t addr_mmsb  = (uint8_t)(addr >> 24);
  uint8_t addr_msb  = (uint8_t)(addr >> 16);
  uint8_t addr_lsb  = (uint8_t)(addr >> 8);
  uint8_t addr_llsb = (uint8_t)(addr & 0xFF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address, reading back status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_mmsb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Send less significant address bytes */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_llsb);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* Flush RX FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  while(len--)
  {
    SSIDataPut(spi->port->base_spi, 0);
    SSIDataGet(spi->port->base_spi, &d);
    *data++ = (uint8_t)d;
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_write32(uint8_t spi_num, uint32_t addr, uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  uint8_t addr_mmsb  = (uint8_t)(addr >> 24);
  uint8_t addr_msb  = (uint8_t)(addr >> 16);
  uint8_t addr_lsb  = (uint8_t)(addr >> 8);
  uint8_t addr_llsb = (uint8_t)(addr & 0xFF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address, reading back status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_mmsb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Send less significant address bytes */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_llsb);

  w = *data;
  SSIDataPut(spi->port->base_spi, w);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstwrite32(uint8_t spi_num, uint32_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  uint8_t addr_mmsb  = (uint8_t)(addr >> 24);
  uint8_t addr_msb  = (uint8_t)(addr >> 16);
  uint8_t addr_lsb  = (uint8_t)(addr >> 8);
  uint8_t addr_llsb = (uint8_t)(addr & 0xFF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address, reading back status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_mmsb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Send less significant address bytes */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_llsb);

  /* Put bytes into TX FIFO */
  while(len--)
  {
    w = *(data++);
    SSIDataPut(spi->port->base_spi, w);
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

/**
 * @}
 */