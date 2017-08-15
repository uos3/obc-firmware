#include "cc112x_spi.h"
#include <stdint.h>

#include "../firmware.h"

rfStatus_t cc112xSpiReadReg(uint8_t spi_device, uint16_t addr, uint8_t *data)
{
  /* Decide what register space is accessed */  
  if((addr & 0xFF00) == 0x0000)
  {
    /* 8-bit register space */
    return SPI_read8(spi_device, (uint8_t)addr, data);
  }
  else
  {
    /* 16-bit register space */
    return SPI_read16(spi_device, addr, data);
  }
}

rfStatus_t cc112xSpiWriteReg(uint8_t spi_device, uint16_t addr, uint8_t *data)
{
  /* Decide what register space is accessed */  
  if((addr & 0xFF00) == 0x0000)
  {
    /* 8-bit register space */
    return SPI_write8(spi_device, (uint8_t)addr, data);
  }
  else
  {
    /* 16-bit register space */
    return SPI_write16(spi_device, addr, data);
  }
}

rfStatus_t cc112xSpiReadBurstReg(uint8_t spi_device, uint16_t addr, uint8_t *data, uint8_t len)
{
  /* Decide what register space is accessed */  
  if((addr & 0xFF00) == 0x0000)
  {
    /* 8-bit register space */
    return SPI_burstread8(spi_device, (uint8_t)addr, data, len);
  }
  else
  {
    /* 16-bit register space */
    return SPI_burstread16(spi_device, addr, data, len);
  }
}

rfStatus_t cc112xSpiWriteBurstReg(uint8_t spi_device, uint16_t addr, uint8_t *data, uint8_t len)
{
  /* Decide what register space is accessed */  
  if((addr & 0xFF00) == 0x0000)
  {
    /* 8-bit register space */
    return SPI_burstwrite8(spi_device, (uint8_t)addr, data, len);
  }
  else
  {
    /* 16-bit register space */
    return SPI_burstwrite16(spi_device, addr, data, len);
  }
}

rfStatus_t cc112xSpiWriteTxFifo(uint8_t spi_device, uint8_t *data, uint8_t len)
{
  return SPI_burstwrite8(spi_device, CC112X_BURST_TXFIFO, data, len);
}

rfStatus_t cc112xSpiReadRxFifo(uint8_t spi_device, uint8_t *data, uint8_t len)
{
  return SPI_burstread8(spi_device, CC112X_BURST_RXFIFO, data, len);
}

rfStatus_t cc112xGetTxStatus(uint8_t spi_device)
{
  return SPI_cmdstrobe(spi_device, CC112X_SNOP);
}

rfStatus_t cc112xGetRxStatus(uint8_t spi_device)
{
  return SPI_cmdstrobe(spi_device, CC112X_SNOP | 0x80);
}
