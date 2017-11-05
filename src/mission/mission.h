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

typedef struct subsystems_ok_t {
	bool eeprom;
	bool fram;
	bool camera;
	bool gps;
	bool imu;
	bool transmitter;
	bool receiver;
	bool eps;
} subsystems_ok_t;

void Mission_Init(void);

void Mission_loop(void);

#endif /*  __MISSION_H__ */