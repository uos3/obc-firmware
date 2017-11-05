/**
 * @defgroup mission
 *
 * @brief Spacecraft Mission Module.
 *
 * @details Spacecraft Mission Module.
 *
 * @ingroup mission
 */

#ifndef __MISSION_H__
#define __MISSION_H__

#include <stdint.h>
#include <stdbool.h>

/* Stored in EEPROM */
typedef struct mission_state_data_t {
	/* 32b word */
	bool mission_inprogress:1;
	bool antenna_deployed:1;
	/* 32b word */
	uint16_t obc_reboots;
	uint16_t configuration_crcfail;
} mission_state_data_t;

typedef struct mission_state_t {
	mission_state_data_t data;
	uint16_t crc;
} mission_state_t;

/* Stored in RAM (volatile) */
typedef struct subsystems_ok_t {
	/* 32b word */
	bool eeprom:1;
	bool fram:1;
	bool camera:1;
	bool gps:1;
	bool imu:1;
	bool transmitter:1;
	bool receiver:1;
	bool eps:1;
} subsystems_ok_t;


mission_state_t mission_state;

subsystems_ok_t subsystems_ok;

void Mission_Init(void);

void Mission_loop(void);

#endif /*  __MISSION_H__ */