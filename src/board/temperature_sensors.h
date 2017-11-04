/**
 * @defgroup temperature_sensors
 *
 * @brief Temperature Sensors Hardware Driver
 *
 * @details Temperature Sensors Hardware Driver.
 *
 * @ingroup drivers
 */

#ifndef __TEMPERATURE_SENSORS_H__
#define __TEMPERATURE_SENSORS_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @function
 * @ingroup temperature_sensors
 *
 * Read temperature value of PCT2075 Sensor.
 *
 * @returns Temperature returned by device
 */
int16_t Temperature_read_pct2075(void);

/**
 * @function
 * @ingroup temperature_sensors
 *
 * Read temperature value of TMP100 Sensor.
 *
 * @returns Temperature returned by device
 */
int16_t Temperature_read_tmp100(void);

#endif /*  __TEMPERATURE_SENSORS_H__ */