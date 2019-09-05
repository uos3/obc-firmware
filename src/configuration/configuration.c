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

  checksum = Util_crc16((uint8_t *)(&(spacecraft_configuration.data)), sizeof(configuration_data_t));

  return (checksum == spacecraft_configuration.checksum);
}

static void Configuration_load_defaults(void)
{
  /* Set defaults.. */
  //OTHERWISE NO CONFIG FILE LOADED IF SAVING CONFIG SAVE FAILS
  spacecraft_configuration.data.tx_enable = 1;              //tx_enabled
  spacecraft_configuration.data.tx_interval = 60;           //60s interval
  spacecraft_configuration.data.tx_interval_downlink = 50;  //500ms
  spacecraft_configuration.data.tx_datarate = 12;           //6kbps
  spacecraft_configuration.data.tx_power = 12;              //300mW

  spacecraft_configuration.data.batt_overtemp = 40;   //40 degrees
  spacecraft_configuration.data.obc_overtemp = 75;    //75 degrees
  spacecraft_configuration.data.pa_overtemp = 75;     //75 degrees

  spacecraft_configuration.data.low_voltage_threshold = 75; //means 7.5 volts as we use integers
  spacecraft_configuration.data.low_voltage_recovery = 78;  //means 7.8 volts as we use integers

  spacecraft_configuration.data.eps_health_acquisition_interval = 300;    //300s period
  spacecraft_configuration.data.check_health_acquisition_interval = 30;   //30s period

  spacecraft_configuration.data.imu_acquisition_interval = 600;           //600s period
  spacecraft_configuration.data.imu_sample_count = 6;                     //6 samples
  spacecraft_configuration.data.imu_sample_interval = 500;                //500ms

  spacecraft_configuration.data.gps_acquisition_interval = 600;           //600s period
  spacecraft_configuration.data.gps_sample_count = 4;                     //max 4 samples possible to fit in telemetry beacon
  spacecraft_configuration.data.gps_sample_interval = 500;                //500ms            

  spacecraft_configuration.data.image_acquisition_profile = 3;            //initial resolution 800x640

  spacecraft_configuration.data.power_rail_1 = 1;     //default ON (Radio TX)
  spacecraft_configuration.data.power_rail_3 = 1;     //default ON (Camera)
  spacecraft_configuration.data.power_rail_5 = 1;     //default ON (GNSS RX)
  spacecraft_configuration.data.power_rail_6 = 1;     //default ON (GNSS LNA)

  spacecraft_configuration.data.imu_accel_enabled = 1;  //default ON
  spacecraft_configuration.data.imu_gyro_enabled = 1;   //default ON
  spacecraft_configuration.data.imu_magno_enabled = 1;  //default ON

  Configuration_update_checksum();
}

static void Configuration_update_checksum(void)
{
  spacecraft_configuration.checksum = Util_crc16((uint8_t *)(&(spacecraft_configuration.data)), sizeof(configuration_data_t));
}

/**
 * @}
 */