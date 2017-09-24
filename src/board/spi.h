/**
 * @defgroup spi
 *
 * @brief SPI Peripheral Driver
 *
 * @details Hardware SPI Peripheral Driver.
 *
 * SPI ports for each board are defined in ${board}/board.h
 *
 * @ingroup drivers
 */

#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @function
 * @ingroup spi
 *
 * Initialises the specified SPI peripheral.
 *
 * @param spi_num ID of the SPI peripheral
 */
void SPI_init(uint8_t spi_num);

/**
 * @function
 * @ingroup spi
 *
 * Reads a byte from an address of the slave device on the specified SPI peripheral.
 *
 * @param spi_num ID of the SPI peripheral
 * @param addr Address of the slave device register
 * @param data Pointer to a byte for storage of the data retrieved
 * @returns Status byte
 */
uint8_t SPI_read8(uint8_t spi_num, uint8_t addr, uint8_t *data);

/**
 * @function
 * @ingroup spi
 *
 * Writes a byte to an address of the slave device on the specified SPI peripheral.
 *
 * @param spi_num ID of the SPI peripheral
 * @param addr Address of the slave device register
 * @param data Pointer to a byte containing the data to be written
 * @returns Status byte
 */
uint8_t SPI_write8(uint8_t spi_num, uint8_t addr, uint8_t *data);

/**
 * @function
 * @ingroup spi
 *
 * Reads a number of bytes from an address of the slave device on the specified SPI peripheral.
 *
 * @param spi_num ID of the SPI peripheral
 * @param addr Address of the slave device register
 * @param data Pointer to bytes for storage of the data retrieved
 * @param len Number of bytes to be read
 * @returns Status byte
 */
uint8_t SPI_burstread8(uint8_t spi_num, uint8_t addr, uint8_t *data, uint16_t len);

/**
 * @function
 * @ingroup spi
 *
 * Writes a number of bytes to an address of the slave device on the specified SPI peripheral.
 *
 * @param spi_num ID of the SPI peripheral
 * @param addr Address of the slave device register
 * @param data Pointer to bytes containing the data to be written
 * @param len Number of bytes to be written
 * @returns Status byte
 */
uint8_t SPI_burstwrite8(uint8_t spi_num, uint8_t addr, uint8_t *data, uint16_t len);

/**
 * @function
 * @ingroup spi
 *
 * Reads a byte from an extended address of the slave device on the specified SPI peripheral.
 *
 * @param spi_num ID of the SPI peripheral
 * @param addr Extended address of the slave device register
 * @param data Pointer to a byte for storage of the data retrieved
 * @returns Status byte
 */
uint8_t SPI_read16(uint8_t spi_num, uint16_t addr, uint8_t *data);

/**
 * @function
 * @ingroup spi
 *
 * Writes a byte to an extended address of the slave device on the specified SPI peripheral.
 *
 * @param spi_num ID of the SPI peripheral
 * @param addr Extended address of the slave device register
 * @param data Pointer to a byte containing the data to be written
 * @returns Status byte
 */
uint8_t SPI_write16(uint8_t spi_num, uint16_t addr, uint8_t *data);

/**
 * @function
 * @ingroup spi
 *
 * Reads a number of bytes from an extended address of the slave device on the specified SPI peripheral.
 *
 * @param spi_num ID of the SPI peripheral
 * @param addr Extended address of the slave device register
 * @param data Pointer to bytes for storage of the data retrieved
 * @param len Number of bytes to be read
 * @returns Status byte
 */
uint8_t SPI_burstread16(uint8_t spi_num, uint16_t addr, uint8_t *data, uint16_t len);

/**
 * @function
 * @ingroup spi
 *
 * Writes a number of bytes to an extended address of the slave device on the specified SPI peripheral.
 *
 * @param spi_num ID of the SPI peripheral
 * @param addr Extended address of the slave device register
 * @param data Pointer to bytes containing the data to be written
 * @param len Number of bytes to be written
 * @returns Status byte
 */
uint8_t SPI_burstwrite16(uint8_t spi_num, uint16_t addr, uint8_t *data, uint16_t len);

/**
 * @function
 * @ingroup spi
 *
 * Sends a TI 'Command Strobe' byte on the specified SPI peripheral
 *
 * @param spi_num ID of the SPI peripheral
 * @param cmd Command byte to be sent
 * @returns Status byte
 */
uint8_t SPI_cmdstrobe(uint8_t spi_num, uint8_t cmd);

#endif /*  __SPI_H__ */