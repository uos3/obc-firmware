/**
 * @ingroup fram
 *
 * @file fram.c
 *
 * @{
 */

#include "../firmware.h"

#define FRAM_OPCODE_WREN  0x06 // Set write enable latch
#define FRAM_OPCODE_WRDI  0x04 // Reset write enable latch

#define FRAM_OPCODE_RDSR  0x05 // Read Status Register
#define FRAM_OPCODE_WRSR  0x01 // Write Status Register

#define FRAM_OPCODE_READ  0x03 // Read memory data
#define FRAM_OPCODE_FSTRD 0x0b // Fast read memory data
#define FRAM_OPCODE_WRITE 0x02 // Write memory data

#define FRAM_OPCODE_SLEEP 0x99 // Enter sleep mode
#define FRAM_OPCODE_RDID  0x9F // Read device ID
#define FRAM_OPCODE_SNR   0xc3 // Read S/N

static uint8_t fram_deviceid[9] = { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xC2, 0x24, 0x00 };

void Fram_init(void)
{
  SPI_init(SPI_FRAM);
}

bool FRAM_selfTest(void)
{
  uint8_t fram_deviceid_read[9];

  Fram_init();

  SPI_burstread8(SPI_FRAM, FRAM_OPCODE_RDID, fram_deviceid_read, 9);

  return !memcmp(fram_deviceid_read, fram_deviceid, 9);
}

void FRAM_write(uint32_t address, uint8_t *data, uint32_t length)
{
  /* Set Write-Enable Latch (Single byte command) */
  SPI_cmd(SPI_FRAM, FRAM_OPCODE_WREN);

  SPI_burstwrite32(SPI_FRAM, ((FRAM_OPCODE_WRITE << 24) | address), data, length);
}

void FRAM_read(uint32_t address, uint8_t *data, uint32_t length)
{
  SPI_burstread32(SPI_FRAM, ((FRAM_OPCODE_READ << 24) | address), data, length);
}

/**
 * @}
 */