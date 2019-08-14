/**
 * @defgroup eps
 *
 * @brief EPS Hardware Driver
 *
 * @details
 *
 * @ingroup eps
 */

#ifndef __EPS_H__
#define __EPS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define EPS_BAUDRATE		    56000

#define EPS_REG_CONFIG			0x00		//8bit
#define EPS_REG_STATUS			0x01		//8bit
#define EPS_REG_SW_ON			0x02		//6bit		//Setting a bit in SW_ON to one toggles the state of the corresponding rail, also used for getting the current state of the rails
#define EPS_REG_RESET			0x03		//0bit (set only)      //Resets (PUC) the EPS by forcing watchdog timeout
#define EPS_REG_BAT_V			0x04		//16bit     //Can reset the battery if set to
#define EPS_REG_BAT_I			0x05		//16bit
#define EPS_REG_BAT_T			0x06		//16bit
#define EPS_REG_SOLAR_POS_X1_I		0x07		//16bit     //using the convention that +X <-> north; -X <-> south
#define EPS_REG_SOLAR_POS_X2_I		0x08		//16bit     //-Y <-> West and so on
#define EPS_REG_SOLAR_NEG_X1_I		0x09		//16bit
#define EPS_REG_SOLAR_NEG_X2_I		0x0A		//16bit
#define EPS_REG_SOLAR_POS_Y1_I		0x0B		//16bit
#define EPS_REG_SOLAR_POS_Y2_I		0x0C		//16bit
#define EPS_REG_SOLAR_NEG_Y1_I		0x0D		//16bit
#define EPS_REG_SOLAR_NEG_Y2_I		0x0E		//16bit
#define EPS_REG_SOLAR_NEG_Z1_I		0x0F		//16bit
#define EPS_REG_SOLAR_NEG_Z2_I		0x10		//16bit
#define EPS_REG_SOLAR_1_V 		0x11		//16bit		 // Used to be reservered, referred to in comm spec
#define EPS_REG_SOLAR_2_V		0x12		//16bit
#define EPS_REG_RAIL1_BOOT		0x13		//16bit
#define EPS_REG_RAIL1_FAIL		0x14		//16bit
#define EPS_REG_RAIL1_V			0x15		//16bit
#define EPS_REG_RAIL1_I			0x16		//16bit
#define EPS_REG_RAIL2_BOOT		0x17		//16bit
#define EPS_REG_RAIL2_FAIL		0x18		//16bit
#define EPS_REG_RAIL2_V			0x19		//16bit
#define EPS_REG_RAIL2_I			0x1A		//16bit
#define EPS_REG_RAIL3_BOOT		0x1B		//16bit
#define EPS_REG_RAIL3_FAIL		0x1C		//16bit
#define EPS_REG_RAIL3_V			0x1D		//16bit
#define EPS_REG_RAIL3_I			0x1E		//16bit
#define EPS_REG_RAIL4_BOOT		0x1F		//16bit
#define EPS_REG_RAIL4_FAIL		0x20		//16bit
#define EPS_REG_RAIL4_V			0x21		//16bit
#define EPS_REG_RAIL4_I			0x22		//16bit
#define EPS_REG_RAIL5_BOOT		0x23		//16bit
#define EPS_REG_RAIL5_FAIL		0x24		//16bit
#define EPS_REG_RAIL5_V			0x25		//16bit
#define EPS_REG_RAIL5_I			0x26		//16bit
#define EPS_REG_RAIL6_BOOT		0x27		//16bit
#define EPS_REG_RAIL6_FAIL		0x28		//16bit
#define EPS_REG_RAIL6_V			0x29		//16bit
#define EPS_REG_RAIL6_I			0x2A		//16bit
#define EPS_REG_SUPPLY_3_V		0x2B		//16bit
#define EPS_REG_SUPPLY_3_I		0x2C		//16bit
#define EPS_REG_SUPPLY_5_V		0x2D		//16bit
#define EPS_REG_SUPPLY_5_I		0x2E		//16bit
#define EPS_REG_CHARGE_I		0x2F		//16bit
#define EPS_REG_MPPT_BUS_V		0x30		//16bit //Battery charge voltage
#define EPS_REG_CRC_ER_CNT		0x31		//16bit	
#define EPS_REG_DROP_CNT		0x32		//16bit
#define EPS_REG_SW_OFF			0x33		//6bit
#define EPS_KNOWN_VAL			0x34        //6bit       //For self testing holds 42

#define EPS_ID			0x42	//The value held by EPS_KNOWN_VAL

#define EPS_PWR_TX      0x01		//The bit positions corresponding with each rail in SW_ON
#define EPS_PWR_RX      0x02
#define EPS_PWR_CAM     0x04
#define EPS_PWR_MCU     0x08
#define EPS_PWR_GPS     0x10
#define EPS_PWR_GPSLNA  0x20

static uint16_t EPS_COMMS_FAILS;	//number of failures of the EPS to respond to communication from the EPS
//TODO add systems to deal with EPS MCU failure
/**
 * @function
 * @ingroup eps
 *
 * Initialise EPS Communication
 *
 */
void EPS_init(void);

/**
 * @function
 * @ingroup eps
 *
 * Verify EPS Communication
 *
 * @returns bool True if communication is successful, False else.
 */
bool EPS_selfTest(void);



//Used for external access of data from EPS, returns true if successful
bool EPS_getInfo(uint16_t *output, uint8_t regID);

//Gets the current state of the power rails
uint8_t EPS_getPowerRail();


//Sends a packet with an incorrect CRC value
bool EPS_testWrongCRC(uint8_t register_id);

//Sends a 4 byte (incomplete) packet to the EPS
bool EPS_testPartialPacket(uint8_t register_id);


//Sets all power rails to the states indicated in regVal
bool EPS_setPowerRail(uint8_t regVal);
//Resets all power rails corresponding with a 1 in regVal
bool EPS_resetPowerRail(uint8_t regVal);

bool EPS_writeRegister(uint8_t register_id, uint8_t register_value);

#endif /* __EPS_H__ */
