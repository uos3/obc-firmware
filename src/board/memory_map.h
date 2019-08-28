// EEPROM
#define EEPROM_REBOOT_COUNT   0x00000000            //variable to store the number of reboots
#define EEPROM_DEPLOY_STATUS 0x00000004             //variable to store the information if the antenna was deployed
#define EEPROM_DWELL_TIME_PASSED 0x00000008         //variable to store the information if the 45 mins dwell time has been completed
#define EEPROM_WAS_LAST_MODE_SAFE 0x0000000C        //variable to store the information if the last mode was safe mode
#define EEPROM_CONFIGURATION_DATA_ADDRESS 0x0100

// FRAM
#define FRAM_AVAILABLE  0
#define FRAM_USED       1

#define FRAM_PAYLOAD_SIZE     0x00069
#define FRAM_TABLE_SIZE       0x00261

#define FRAM_TABLE_OFFSET     0x00000 // 609 bytes (4870 bits)
#define FRAM_PAYLOAD_OFFSET   0x00261 // 4870 packets capacity