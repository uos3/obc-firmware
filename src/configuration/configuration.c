/**
 * @ingroup configuration
 *
 * @file configuration.c
 *
 * @{
 */

#include "../firmware.h"

static void Configuration_save_to_eeprom(void);
static bool Configuration_load_from_eeprom(void);
static void Configuration_load_defaults(void);
static void Configuration_update_checksum(void);

void Configuration_Init(void)
{
  if(Configuration_load_from_eeprom() == false)
  {
    /* Load defaults and save EEPROM copy */
    Configuration_load_defaults();
    Configuration_save_to_eeprom();
  }



}

static void Configuration_save_to_eeprom(void)
{
  EEPROM_write(EEPROM_CONFIGURATION_DATA_ADDRESS, (uint32_t*)(&spacecraft_configuration), sizeof(configuration_t));
}

static bool Configuration_load_from_eeprom(void)
{
  if(EEPROM_selfTest() == false)
    return false;

  EEPROM_read(EEPROM_CONFIGURATION_DATA_ADDRESS, (uint32_t*)(&spacecraft_configuration), sizeof(configuration_t));

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