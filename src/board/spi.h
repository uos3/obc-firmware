#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include <stdbool.h>

void SPI_init(uint8_t spi_num);

uint8_t SPI_read8(uint8_t spi_num, uint8_t addr, uint8_t *data);
uint8_t SPI_write8(uint8_t spi_num, uint8_t addr, uint8_t *data);

uint8_t SPI_burstread8(uint8_t spi_num, uint8_t addr, uint8_t *data, uint16_t len);
uint8_t SPI_burstwrite8(uint8_t spi_num, uint8_t addr, uint8_t *data, uint16_t len);

uint8_t SPI_read16(uint8_t spi_num, uint16_t addr, uint8_t *data);
uint8_t SPI_write16(uint8_t spi_num, uint16_t addr, uint8_t *data);

uint8_t SPI_burstread16(uint8_t spi_num, uint16_t addr, uint8_t *data, uint16_t len);
uint8_t SPI_burstwrite16(uint8_t spi_num, uint16_t addr, uint8_t *data, uint16_t len);

uint8_t SPI_cmdstrobe(uint8_t spi_num, uint8_t cmd);

#endif /*  __SPI_H__ */