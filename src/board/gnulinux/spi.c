/**
 * @ingroup gnulinux
 * @ingroup spi
 *
 * @file gnulinux/spi.c
 * @brief SPI Driver - gnulinux platform
 *
 * @{
 */

#include "../spi.h"

/** Public Functions */

void SPI_init(uint8_t spi_num)
{
  (void) spi_num;
}

uint8_t SPI_cmdstrobe(uint8_t spi_num, uint8_t cmd)
{
  (void) spi_num;
  (void) cmd;

  return 0;
}

uint8_t SPI_read8(uint8_t spi_num, uint8_t addr, uint8_t *data)
{
  (void) spi_num;
  (void) addr;
  (void) data;

  return 0;
}

uint8_t SPI_burstread8(uint8_t spi_num, uint8_t addr, uint8_t *data, uint16_t len)
{
  (void) spi_num;
  (void) addr;
  (void) data;
  (void) len;

  return 0;
}

uint8_t SPI_write8(uint8_t spi_num, uint8_t addr, uint8_t *data)
{
  (void) spi_num;
  (void) addr;
  (void) data;

  return 0;
}

uint8_t SPI_burstwrite8(uint8_t spi_num, uint8_t addr, uint8_t *data, uint16_t len)
{
  (void) spi_num;
  (void) addr;
  (void) data;
  (void) len;

  return 0;
}

uint8_t SPI_read16(uint8_t spi_num, uint16_t addr, uint8_t *data)
{
  (void) spi_num;
  (void) addr;
  (void) data;

  return 0;
}

uint8_t SPI_burstread16(uint8_t spi_num, uint16_t addr, uint8_t *data, uint16_t len)
{
  (void) spi_num;
  (void) addr;
  (void) data;
  (void) len;

  return 0;
}

uint8_t SPI_write16(uint8_t spi_num, uint16_t addr, uint8_t *data)
{
  (void) spi_num;
  (void) addr;
  (void) data;

  return 0;
}

uint8_t SPI_burstwrite16(uint8_t spi_num, uint16_t addr, uint8_t *data, uint16_t len)
{
  (void) spi_num;
  (void) addr;
  (void) data;
  (void) len;

  return 0;
}

/**
 * @}
 */