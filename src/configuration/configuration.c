/**
 * @ingroup configuration
 *
 * @file configuration.c
 *
 * @{
 */

#include "../firmware.h"

static void Configuration_update_checksum(void);

void Configuration_Init(void)
{
  /* Set defaults.. */



  Configuration_update_checksum();
}

bool Configuration_verify_checksum(void)
{
  uint16_t checksum;
  
  Packet_crc16((uint8_t *)(&(spacecraft_configuration.data)), sizeof(configuration_data_t), &checksum);

  return (checksum == spacecraft_configuration.checksum);
}

static void Configuration_update_checksum(void)
{
  Packet_crc16((uint8_t *)(&(spacecraft_configuration.data)), sizeof(configuration_data_t), &(spacecraft_configuration.checksum));
}

/**
 * @}
 */