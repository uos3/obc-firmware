/* 
	=== TEMPORARY ===
	I would advise breaking this up into individual component-based
	dependancies, or even putting them in the component files.
*/
#include <string.h>
#include <stdint.h>

#include "../utility/byte_plexing.h"

#include "../component/eps.h"
#include "../component/gnss.h"

#include "mode_base.h"
#include "mission_tasks.h"
#include "packet_application.h"
#include "packet.h"


void task_health_check(void) {
	// never got to this
}


void task_save_eps_health(void) {
	// check EPS is reachable
	if (!EPS_selfTest){
		return;
	}
	// request the data from the EPS
}


void task_save_gps_data(void) {
	int offset = 0;
	size_t max_size;
	int recieved_data_from_gnss;
	uint32_t seconds = 0;
	uint16_t week = 0;
	int32_t longitude=0, latitude=0, altitude=0;
	uint8_t long_sd=0, lat_sd=0, alt_sd=0;

	// would cut down on file size a lot if this was moved the GNSS driver...
	// define an array to copy the data to
	max_size = 	\
		sizeof(longitude) + \
		sizeof(latitude) + \
		sizeof(altitude) + \
		sizeof(long_sd) + \
		sizeof(lat_sd) + \
		sizeof(alt_sd) + \
		sizeof(week) + \
		sizeof(seconds);
	uint8_t concatenated_data[max_size];

	// pull the data from the GNSS, if it is available
	recieved_data_from_gnss = GNSS_getData(
		&longitude,
		&latitude,
		&altitude,
		&long_sd,
		&lat_sd,
		&alt_sd,
		&week,
		&seconds
	);

	if (recieved_data_from_gnss == 0){
		return;
	}

	copy_to_array(concatenated_data, max_size, &offset, &longitude, sizeof(longitude));
	copy_to_array(concatenated_data, max_size, &offset, &latitude, sizeof(latitude));
	copy_to_array(concatenated_data, max_size, &offset, &altitude, sizeof(altitude));
	copy_to_array(concatenated_data, max_size, &offset, &long_sd,	sizeof(long_sd));
	copy_to_array(concatenated_data, max_size, &offset, &lat_sd, sizeof(lat_sd));
	copy_to_array(concatenated_data, max_size, &offset, &alt_sd, sizeof(alt_sd));
	copy_to_array(concatenated_data, max_size, &offset, &seconds,	sizeof(seconds));
	copy_to_array(concatenated_data, max_size, &offset, &week, sizeof(week));

	store_payload_data(WHOFOR_UART_GNSS, cast_asptr(concatenated_data), max_size);
}

void task_save_attitude(void) {
}


void task_transmit_buffer_data(void) {
}


void task_exit_bu(void) {

}


void task_transmit_morse_telemetry(void) {

}


void task_transmit_packet_telemetry(void) {

}


void task_deploy_antenna(void) {

}


void task_take_picture(void) {

}


void task_process_gs_signal(void) {

}

