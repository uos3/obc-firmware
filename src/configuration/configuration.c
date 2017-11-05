/**
 * @ingroup configuration
 *
 * @file configuration.c
 *
 * @{
 */

#include "../firmware.h"

#define CONFIGURATION_EEPROM_ADDRESS 0x0000

static bool Configuration_load_eeprom(void);
static void Configuration_load_defaults(void);
static void Configuration_update_checksum(void);

void Configuration_Init(void)
{
  if(Configuration_load_eeprom() == false)
    Configuration_load_defaults();



}

static bool Configuration_load_eeprom(void)
{
  if(EEPROM_selfTest() == false)
    return false;

  EEPROM_read(CONFIGURATION_EEPROM_ADDRESS, (uint32_t*)(&spacecraft_configuration), sizeof(configuration_t));

  return Configuration_verify_checksum();
}

bool Configuration_verify_checksum(void)
{
  uint16_t checksum;

  Packet_crc16((uint8_t *)(&(spacecraft_configuration.data)), sizeof(configuration_data_t), &checksum);

  return (checksum == spacecraft_configuration.checksum);
}

static void Configuration_load_defaults(void)
{
  /* Set defaults.. */


  Configuration_update_checksum();
}

static void Configuration_update_checksum(void)
{
  Packet_crc16((uint8_t *)(&(spacecraft_configuration.data)), sizeof(configuration_data_t), &(spacecraft_configuration.checksum));
}

/**
 * @}
 */