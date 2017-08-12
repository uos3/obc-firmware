#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include <stdbool.h>

void SPI_init(uint8_t spi_num);

uint8_t SPI_read8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData);
uint8_t SPI_burstread8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len);

uint8_t SPI_write8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData);
uint8_t SPI_burstwrite8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len);

uint8_t SPI_cmdstrobe(uint8_t spi_num, uint8_t cmd, uint32_t *rc);

#endif /*  __SPI_H__ */