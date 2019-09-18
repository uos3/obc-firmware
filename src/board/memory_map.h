/* EEPROM memory map */

/* Space defined for the variables which need to be retrieved after potential reboot */
#define EEPROM_DEPLOY_STATUS                    0x00000000      //variable to store the information if the antenna was deployed
#define EEPROM_DWELL_TIME_PASSED                0x00000004      //variable to store the information if the 45 mins dwell time has been completed
#define EEPROM_RESERVED                         0x00000008      //free to add something
#define EEPROM_ADM_DEPLOY_ATTEMPTS              0x0000000C      //variable to store the information about the number of deploy attempts
#define EEPROM_REBOOT_COUNT                     0x00000010      //variable to store the number of reboots
#define EEPROM_CURRENT_MODE                     0x00000014      //variable to store the information about the satellite is in
#define EEPROM_REBOOT_COUNT_NF                  0x00000018      //variable to store the information about the number of reboots in NF mode
#define EEPROM_REBOOT_COUNT_DL                  0x0000001C      //variable to store the information about the number of reboots in DL mode
#define EEPROM_REBOOT_COUNT_LP                  0x00000020      //variable to store the information about the number of reboots in LP mode
#define EEPROM_REBOOT_COUNT_SM                  0x00000024      //variable to store the information about the number of reboots in SM mode

/* Layout of the EEPROM for the configuration data - there will be three copies of the configuration file */
/* If the CRC of the main copy will be not valid - back_up copies will be used to replace it */
/* Distance between each copies is 256bits - it should be enough as current size of the configuration is around 170bits */
#define EEPROM_CONFIGURATION_DATA_ADDRESS_MAIN          0x00000100      //main copy of the cofiguration data
#define EEPROM_CONFIGURATION_DATA_ADDRESS_BACKUP_ONE    0x00000300      //first back_up copy of the cofiguration data
#define EEPROM_CONFIGURATION_DATA_ADDRESS_BACKUP_TWO    0x00000500      //second back_up copy of the cofiguration data
/* received configuration file is first stored in the EEPROM when processing the gc_command and after that in the CFU mode is verfied */
#define EEPROM_RECEIVED_CONFIGURATION_FILE              0x00000700      //configuration data received from the gc_command, for its temporary storage

// FRAM
#define FRAM_AVAILABLE  0
#define FRAM_USED       1

#define FRAM_PAYLOAD_SIZE     0x00069
#define FRAM_TABLE_SIZE       0x00261

#define FRAM_TABLE_OFFSET     0x00000 // 609 bytes (4870 bits)
#define FRAM_PAYLOAD_OFFSET   0x00261 // 4870 packets capacity