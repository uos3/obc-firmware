/**
 * @ingroup uos3-proto
 * @ingroup temperature_sensors
 *
 * @file uos3-proto/temperature_sensors.c
 * @brief Temperature Sensors Hardware Driver - uos3-proto board
 *
 * @{
 */

#include "board.h"
#include "../i2c.h"

#include "../temperature_sensors.h"

int16_t Temperature_read_pct2075(void)
{
  // radio amplifier temperature slave address 1001 000 (to 1001 111) depending on pins
  // register with temp data is 00, others are configuration, hysteresis, overtemperature shutdown, tilde (sampling rate) and temp conversion
  // temp is 2 byte, MSB then LSB, 2s complement. each is .125C bottom 5 bits ignored
  // to read 1. scl sda free, send slave address, pointer ,write bit
  // then read slave address, then receive 2 bytes

  const unsigned char pct2075_address=0x48; // %1001000;
  int16_t data = (int16_t)(I2CReceive16(I2C_TEMP, pct2075_address, 0)>>5); // for temp in C divide by 8 - bottom 5 bits worthless
  return data;
}

int16_t Temperature_read_tmp100(void)
{
  // radio amplifier temperature slave address 1001 000 (or 1001001 or 1001010) depending on pins
  // register with temp data is 00, others are configuration, tlow and thigh
  // temp is 2 byte, MSB then LSB, 2s complement. each is .125C bottom 5 bits ignored
  // to read 1. scl sda free, send slave address, pointer ,write bit
  // then read slave address, then receive 2 bytes

  const unsigned char tmp100_address=0x49; // %1001000;
  int16_t data = (int16_t)(I2CReceive16(I2C_TEMP, tmp100_address, 0)>>5); // for temp in C divide by 8 - bottom 5 bits worthless
  return data;
}