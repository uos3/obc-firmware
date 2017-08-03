#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include <stdbool.h>

void SPI_init(uint8_t spi_num, uint32_t clockrate);

uint8_t SPI_read8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len);
uint8_t SPI_write8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len);

uint8_t SPI_read16(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len);
uint8_t SPI_write16(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len);

#endif /*  __SPI_H__ */