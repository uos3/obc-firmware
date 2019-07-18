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

#define EPS_REG_CONFIG			0x00
#define EPS_REG_STATUS			0x01
#define EPS_REG_SW_ON				0x02		//Setting a bit in SW_ON to one toggles the state of the corresponding rail, also used for getting the current state of the rails
#define EPS_REG_POWER				0x03
#define EPS_REG_BAT_V				0x04
#define EPS_REG_BAT_I				0x05
#define EPS_REG_BAT_T				0x06
#define EPS_REG_SOLAR_N1_I	0x07
#define EPS_REG_SOLAR_N2_I	0x08
#define EPS_REG_SOLAR_S1_I	0x09
#define EPS_REG_SOLAR_S2_I	0x0A
#define EPS_REG_SOLAR_E1_I	0x0B
#define EPS_REG_SOLAR_E2_I	0x0C
#define EPS_REG_SOLAR_W1_I	0x0D
#define EPS_REG_SOLAR_W2_I	0x0E
#define EPS_REG_SOLAR_T1_I	0x0F
#define EPS_REG_SOLAR_T2_I	0x10
#define EPS_REG_SOLAR_1_V 		0x11 // Used to be reservered, referred to in comm spec
#define EPS_REG_SOLAR_2_V		0x12
#define EPS_REG_RAIL1_BOOT	0x13
#define EPS_REG_RAIL1_FAIL	0x14
#define EPS_REG_RAIL1_V			0x15
#define EPS_REG_RAIL1_I			0x16
#define EPS_REG_RAIL2_BOOT	0x17
#define EPS_REG_RAIL2_FAIL	0x18
#define EPS_REG_RAIL2_V			0x19
#define EPS_REG_RAIL2_I			0x1A
#define EPS_REG_RAIL3_BOOT	0x1B
#define EPS_REG_RAIL3_FAIL	0x1C
#define EPS_REG_RAIL3_V			0x1D
#define EPS_REG_RAIL3_I			0x1E
#define EPS_REG_RAIL4_BOOT	0x1F
#define EPS_REG_RAIL4_FAIL	0x20
#define EPS_REG_RAIL4_V			0x21
#define EPS_REG_RAIL4_I			0x22
#define EPS_REG_RAIL5_BOOT	0x23
#define EPS_REG_RAIL5_FAIL	0x24
#define EPS_REG_RAIL5_V			0x25
#define EPS_REG_RAIL5_I			0x26
#define EPS_REG_RAIL6_BOOT	0x27
#define EPS_REG_RAIL6_FAIL	0x28
#define EPS_REG_RAIL6_V			0x29
#define EPS_REG_RAIL6_I			0x2A
#define EPS_REG_SUPPLY_3_V	0x2B		//System rails?
#define EPS_REG_SUPPLY_3_I	0x2C
#define EPS_REG_SUPPLY_5_V	0x2D
#define EPS_REG_SUPPLY_5_I	0x2E
#define EPS_REG_CHARGE_I		0x2F
#define EPS_REG_MPPT_BUS_V	0x30
#define EPS_REG_CRC_ER_CNT	0x31		
#define EPS_REG_DROP_CNT		0x32
#define EPS_REG_SW_OFF		0x33
#define EPS_KNOWN_VAL		0x34

#define EPS_PWR_RADIO      0x01		//The bit positions corresponding with each rail in SW_ON
#define EPS_PWR_BURADIO      0x02
#define EPS_PWR_CAM     0x04
#define EPS_PWR_MCU     0x08
#define EPS_PWR_GPS     0x10
#define EPS_PWR_GPSLNA  0x20
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

/**
 * @function
 * @ingroup eps
 *
 * Read Battery Voltage
 *
 * @parameter uint16_t* Pointer to store result.
 * @returns bool True if transaction is successful, False else.
 */

bool EPS_getBatteryInfo(uint16_t *voltage, uint8_t type);


bool EPS_setPowerRail(uint8_t type, bool on);

#endif /* __EPS_H__ */
