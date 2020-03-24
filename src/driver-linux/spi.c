
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../driver/rtc.h"

uint8_t SPI_cmd(const uint8_t spi_num, const uint8_t cmd){}

uint8_t SPI_read8(const uint8_t spi_num, const uint8_t addr, uint8_t *data){}

uint8_t SPI_write8(const uint8_t spi_num, const uint8_t addr, const uint8_t *data){}

uint8_t SPI_burstread8(const uint8_t spi_num, const uint8_t addr, uint8_t *data, uint32_t len){}

uint8_t SPI_burstwrite8(const uint8_t spi_num, const uint8_t addr, uint8_t *data, uint32_t len){}

uint8_t SPI_read16(const uint8_t spi_num, const uint16_t addr, uint8_t *data){}

uint8_t SPI_write16(const uint8_t spi_num, const uint16_t addr, const uint8_t *data){}

uint8_t SPI_burstread16(const uint8_t spi_num, const uint16_t addr, uint8_t *data, uint32_t len){}

uint8_t SPI_burstwrite16(const uint8_t spi_num, const uint16_t addr, uint8_t *data, uint32_t len){}

uint8_t SPI_read32(const uint8_t spi_num, const uint32_t addr, uint8_t *data){}

uint8_t SPI_write32(const uint8_t spi_num, const uint32_t addr, const uint8_t *data){}

uint8_t SPI_burstread32(const uint8_t spi_num, const uint32_t addr, uint8_t *data, uint32_t len){}

uint8_t SPI_burstwrite32(const uint8_t spi_num, const uint32_t addr, uint8_t *data, uint32_t len){}
