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

#define EPS_BAUDRATE		57600

#define EPS_REG_CONFIG			0x00		//8bit
#define EPS_REG_STATUS			0x01		//8bit
#define EPS_REG_SW_ON			0x02		//6bit		//Setting a bit in SW_ON to one toggles the state of the corresponding rail, also used for getting the current state of the rails
#define EPS_REG_POWER			0x03		//6bit
#define EPS_REG_BAT_V			0x04		//16bit
#define EPS_REG_BAT_I			0x05		//16bit
#define EPS_REG_BAT_T			0x06		//16bit
#define EPS_REG_SOLAR_N1_I		0x07		//16bit
#define EPS_REG_SOLAR_N2_I		0x08		//16bit
#define EPS_REG_SOLAR_S1_I		0x09		//16bit
#define EPS_REG_SOLAR_S2_I		0x0A		//16bit
#define EPS_REG_SOLAR_E1_I		0x0B		//16bit
#define EPS_REG_SOLAR_E2_I		0x0C		//16bit
#define EPS_REG_SOLAR_W1_I		0x0D		//16bit
#define EPS_REG_SOLAR_W2_I		0x0E		//16bit
#define EPS_REG_SOLAR_T1_I		0x0F		//16bit
#define EPS_REG_SOLAR_T2_I		0x10		//16bit
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
#define EPS_REG_MPPT_BUS_V		0x30		//16bit
#define EPS_REG_CRC_ER_CNT		0x31		//16bit	
#define EPS_REG_DROP_CNT		0x32		//16bit
#define EPS_REG_SW_OFF			0x33		//6bit
#define EPS_KNOWN_VAL			0x34

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
 */`
bool EPS_selfTest(void);

/**
 * @function
 * @ingroup eps
 *
 * Read Battery Voltage
 *
 * @parameter uint16_t* Pointer to store result.
 * @returns bool True if transaction is successful, False else.
 */

bool EPS_getInfo(uint16_t *voltage, uint8_t type);

uint8_t EPS_getPowerRail();

bool EPS_setPowerRail(uint8_t regVal);

#endif /* __EPS_H__ */
