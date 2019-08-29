// EEPROM
#define EEPROM_DEPLOY_STATUS                0x00000000      //variable to store the information if the antenna was deployed
#define EEPROM_DWELL_TIME_PASSED            0x00000004      //variable to store the information if the 45 mins dwell time has been completed
#define EEPROM_RESERVED                     0x00000008      //free to add something
#define EEPROM_ADM_DEPLOY_ATTEMPTS          0x0000000C      //variable to store the information about the number of deploy attempts
#define EEPROM_REBOOT_COUNT                 0x00000010      //variable to store the number of reboots
#define EEPROM_CURRENT_MODE                 0x00000014      //variable to store the information about the satellite is in
#define EEPROM_REBOOT_COUNT_NF              0x00000018      //variable to store the information about the number of reboots in NF mode
#define EEPROM_REBOOT_COUNT_DL              0x0000001C      //variable to store the information about the number of reboots in DL mode
#define EEPROM_REBOOT_COUNT_LP              0x00000020      //variable to store the information about the number of reboots in LP mode
#define EEPROM_REBOOT_COUNT_SM              0x00000024      //variable to store the information about the number of reboots in SM mode
#define EEPROM_CONFIGURATION_DATA_ADDRESS   0x00000100

// FRAM
#define FRAM_AVAILABLE  0
#define FRAM_USED       1

#define FRAM_PAYLOAD_SIZE     0x00069
#define FRAM_TABLE_SIZE       0x00261

#define FRAM_TABLE_OFFSET     0x00000 // 609 bytes (4870 bits)
#define FRAM_PAYLOAD_OFFSET   0x00261 // 4870 packets capacity