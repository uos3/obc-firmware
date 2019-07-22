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

/* TODO: Move these below into the relevant modules */

typedef enum {
	tx_interval_downlink_0ms,
	tx_interval_downlink_500ms,
	tx_interval_downlink_600ms,
	tx_interval_downlink_700ms,
	tx_interval_downlink_800ms,
	tx_interval_downlink_900ms,
	tx_interval_downlink_1000ms,
	tx_interval_downlink_1500ms,
	tx_interval_downlink_2000ms
} tx_interval_downlink_t;

typedef enum {
	tx_datarate_0_25kbps,
	tx_datarate_0_5kbps,
	tx_datarate_1_kbps,
	tx_datarate_3_kbps,
	tx_datarate_6_kbps
} tx_datarate_t;

typedef enum {
	tx_power_10mw,
	tx_power_50mw,
	tx_power_100mw,
	tx_power_200mw,
	tx_power_300mw
} tx_power_t;

typedef enum {
	image_acquisition_profile_1600x1200,
	//image_acquisition_profile_1280x960,
	//image_acquisition_profile_1024x768,
	image_acquisition_profile_800x600,
	//image_acquisition_profile_640x480,
	//image_acquisition_profile_320x240,
	//image_acquisition_profile_160x120
} image_acquisition_profile_t;

typedef enum {
	operational_mode_deployment,
	operational_mode_nominal,
	operational_mode_safe
} operational_mode_t;

/* Refer to https://groupsite.soton.ac.uk/Academic/BLAST-cubesat/_layouts/xlviewer.aspx?id=/Academic/BLAST-cubesat/Documents/DATA%20PACKAGE%20ATRR/UoS3_SWCONFIG_%2020180618_v1.0.xlsx&Source=https%3A%2F%2Fgroupsite%2Esoton%2Eac%2Euk%2FAcademic%2FBLAST-cubesat%2FDocuments%2FForms%2FAllItems%2Easpx%3FRootFolder%3D%252FAcademic%252FBLAST%252Dcubesat%252FDocuments%252FDATA%2520PACKAGE%2520ATRR%26FolderCTID%3D0x012000F5659642E01623428EAA32D5F7E646E6%26View%3D%7BC7870BB4-9FEA-4635-9448-FF2BD31AD23C%7D%26InitialTabId%3DRibbon%252EDocument%26VisibilityContext%3DWSSTabPersistence&DefaultItemOpen=1 */
/* Packing work in progress */
typedef struct configuration_data_t {
	bool tx_enable:1;

	unsigned tx_interval : 8;
	tx_interval_downlink_t tx_interval_downlink : 8;

	tx_datarate_t tx_datarate : 4;
	tx_power_t tx_power : 4;

	// Upper temperature limits before entering low-power mode
	unsigned tx_overtemp : 8;
	unsigned rx_overtemp : 8;
	unsigned batt_overtemp : 8;
	unsigned obc_overtemp : 8;
	unsigned pa_overtemp : 8;

	// Battery voltage
	unsigned low_voltage_threshold : 8;
	unsigned low_voltage_recovery : 8;

	// Time intervals for sampling and saving
	unsigned health_acquisition_interval : 16;
	unsigned configuration_acquisition_interval : 16;

	// Inertial Measurement paramters
	unsigned imu_acquisition_interval : 16;
	unsigned imu_sample_count : 4;
	unsigned imu_sample_interval : 8;

	// GPS measurement parameters
	unsigned gps_acquisition_interval : 16;
	unsigned gps_sample_count : 4;
	unsigned gps_sample_interval : 8;

	// Camera settings
	unsigned image_acquisition_time : 32;
	image_acquisition_profile_t image_acquisition_profile : 1;

	unsigned time : 32; //unix epoch

	// Transfer between modes
	operational_mode_t operational_mode : 2;
	unsigned self_test : 1;

	// Manually power on all rails apart form 4 which is the on-board computer
	unsigned power_rail_1 : 1;
	unsigned power_rail_2 : 1;
	unsigned power_rail_3 : 1;
	unsigned power_rail_5 : 1;
	unsigned power_rail_6 : 1;

	// Setting to True resets one of these power rails
	unsigned reset_power_rail_1 : 1;
	unsigned reset_power_rail_2 : 1;
	unsigned reset_power_rail_3 : 1;
	unsigned reset_power_rail_4 : 1;
	unsigned reset_power_rail_5 : 1;
	unsigned reset_power_rail_6 : 1;

	// -- Previous/need revisiting
	// Seperate control for telecom board components in case of error
	bool imu_accel_enabled:1;
	bool imu_gyro_enabled:1;
	bool imu_magno_enabled:1;

	tx_power_t tx_overtemp_power:4; // adjustable low-power for safe-mode?

	uint16_t greetings_message_transmission_interval; // packets

} configuration_data_t;

typedef struct configuration_t {
	configuration_data_t data;
	uint16_t checksum;
} configuration_t;


configuration_t spacecraft_configuration;

void Configuration_Init(void);

bool Configuration_verify_checksum(void);

static void Configuration_load_defaults(void);

//static void Configuration_update_checksum(void);

#endif /*  __CONFIGURATION_H__ */