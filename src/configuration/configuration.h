/**
 * @defgroup configuration
 *
 * @brief Spacecraft Configuration Module.
 *
 * @details Spacecraft Configuration Module.
 *
 * @ingroup modules
 */

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	tx_datarate_6kbps,
	tx_datarate_3kbps,
	tx_datarate_1kbps,
	tx_datarate_05kbps,
	tx_datarate_025kbps
} tx_datarate_t;

typedef enum {
	tx_power_300mw, 
	tx_power_200mw, 
	tx_power_100mw, 
	tx_power_50mw, 
	tx_power_10mw
} tx_power_t;

typedef enum {
	image_acquisition_profile_1600x1200_10,
	image_acquisition_profile_640x480_10
} image_acquisition_profile_t;

typedef enum {
	imu_bandwidth_5hz_filter,
	imu_bandwidth_10hz_filter
} imu_bandwidth_t;

/* Refer to https://docs.google.com/spreadsheets/d/1OFgH6SIJJw-ISLHxVPTY4VEa49qBMccs4Qu1e_zjqvg/ */
/* Booleans will be bit-packed only if needed to fit */
typedef struct configuration_data_t {
	bool mission_inprogress;
	bool antennas_deployed;
	
	bool tx_enable;
	uint8_t tx_interval;

	tx_datarate_t tx_datarate;
	tx_power_t tx_power;
	tx_power_t tx_overtemp_power;
	uint8_t tx_overtemp_threshold;

	uint8_t low_voltage_threshold;
	uint8_t low_voltage_recovery;

	bool enable_gps_time_sync;

	uint16_t health_acquisition_interval; // seconds
	uint16_t configuration_acquisition_interval; // seconds
	uint16_t imu_acquisition_interval; // seconds
	uint16_t gps_acquisition_interval; // seconds
	uint32_t image_acquisition_time; // unix epoch time

	bool	imu_accel_enabled;
	bool	imu_gyro_enabled;
	bool	imu_magno_enabled;
	uint8_t imu_sample_count;
	uint8_t imu_sample_interval; // *10ms
	imu_bandwidth_t imu_bandwidth;

	uint8_t gps_sample_count;
	uint8_t gps_sample_interval; // seconds

	image_acquisition_profile_t	image_acquisition_profile;

	uint16_t greetings_message_transmission_interval; // seconds
} configuration_data_t;

typedef struct configuration_t {
	configuration_data_t data;
	uint16_t checksum;
} configuration_t;


configuration_t spacecraft_configuration;

void Configuration_Init(void);

bool Configuration_verify_checksum(void);

#endif /*  __CONFIGURATION_H__ */